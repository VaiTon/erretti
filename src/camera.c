#include <omp.h>
#include <stdio.h>

#include "camera.h"
#include "hit_record.h"
#include "ray.h"
#include "scatter.h"
#include "utils.h"
#include "vec3.h"
#include "world.h"

void camera_default(camera_t *c) {
  c->image_width  = 800;
  c->image_height = 400;

  c->samples_per_pixel = 10;
  c->max_depth         = 10;

  c->vfov     = 90;                   // Vertical view angle (field of view)
  c->lookfrom = (point3_t){0, 0, 0};  // Point camera is looking from
  c->lookat   = (point3_t){0, 0, -1}; // Point camera is looking at
  c->vup      = (point3_t){0, 1, 0};  // Camera-relative "up" direction

  c->defocus_angle = 0;  // Variation angle of rays through each pixel
  c->focus_dist    = 10; // Distance from camera lookfrom point to plane of perfect focus
}

void camera_initialize(camera_t *c) {
  c->aspect_ratio = (double)c->image_width / c->image_height;

  c->_pixel_samples_scale = 1.0 / c->samples_per_pixel;
  c->_center              = c->lookfrom;

  // Determine viewport dimensions.
  double theta           = degrees_to_radians(c->vfov);
  double h               = tan(theta / 2);
  double viewport_height = 2 * h * c->focus_dist;
  double viewport_width  = viewport_height * ((double)c->image_width / c->image_height);

  // Calculate the u,v,w unit basis vectors for the camera coordinate frame.
  c->_w = vec3_unit_vector(vec3_sub(c->lookfrom, c->lookat));
  c->_u = vec3_unit_vector(vec3_cross(c->vup, c->_w));
  c->_v = vec3_cross(c->_w, c->_u);

  // Calculate the vectors across the horizontal and down the vertical viewport edges.
  vec3_t viewport_u = vec3_mul_sc(c->_u, viewport_width); // Vector across viewport horizontal edge
  vec3_t viewport_v = vec3_mul_sc(c->_v, -viewport_height); // Vector down viewport vertical edge

  // Calculate the horizontal and vertical delta vectors from pixel to pixel.
  c->_pixel_delta_u = vec3_div_sc(viewport_u, c->image_width);
  c->_pixel_delta_v = vec3_div_sc(viewport_v, c->image_height);

  vec3_t half_viewport_u = vec3_div_sc(viewport_u, 2);
  vec3_t half_viewport_v = vec3_div_sc(viewport_v, 2);

  vec3_t viewport_upper_left;
  {
    vec3_t ul1 = vec3_sub(c->_center, vec3_mul_sc(c->_w, c->focus_dist));
    vec3_t ul2 = vec3_sub(ul1, half_viewport_u);
    vec3_t ul3 = vec3_sub(ul2, half_viewport_v);

    viewport_upper_left = ul3;
  }

  {
    vec3_t p00      = vec3_add(c->_pixel_delta_u, c->_pixel_delta_v);
    vec3_t p01      = vec3_mul_sc(p00, 0.5);
    vec3_t p02      = vec3_add(viewport_upper_left, p01);
    c->_pixel00_loc = p02;
  }

  // Calculate the camera defocus disk basis vectors.
  {
    double defocus_radius = c->focus_dist * tan(degrees_to_radians(c->defocus_angle / 2));
    c->_defocus_disk_u    = vec3_mul_sc(c->_u, defocus_radius);
    c->_defocus_disk_v    = vec3_mul_sc(c->_v, defocus_radius);
  }
}

vec3_t sample_square() {
  // Returns the vector to a random point in the [-.5,-.5]-[+.5,+.5] unit square.
  return (vec3_t){random_double() - 0.5, random_double() - 0.5, 0};
}

vec3_t sample_disk(const double radius) {
  // Returns a random point in the unit (radius 0.5) disk centered at the origin.
  return vec3_mul_sc(vec3_rand_unit_disk(), radius);
}

point3_t defocus_disk_sample(camera_t *c) {
  // Returns a random point in the camera defocus disk.
  vec3_t p           = vec3_rand_unit_disk();
  vec3_t u_component = vec3_mul_sc(c->_defocus_disk_u, p.x);
  vec3_t v_component = vec3_mul_sc(c->_defocus_disk_v, p.y);

  vec3_t sample = vec3_add(v_component, u_component);
  sample        = vec3_add(sample, c->_center);

  return sample;
}

// Construct a camera ray originating from the defocus disk and directed at a randomly
// sampled point around the pixel location i, j.
ray_t camera_get_ray(camera_t *c, const size_t i, const size_t j) {

  vec3_t offset = sample_square();

  vec3_t pixel_sample = c->_pixel00_loc;
  pixel_sample        = vec3_add(pixel_sample, vec3_mul_sc(c->_pixel_delta_u, offset.x + i));
  pixel_sample        = vec3_add(pixel_sample, vec3_mul_sc(c->_pixel_delta_v, offset.y + j));

  vec3_t ray_origin    = (c->defocus_angle <= 0) ? c->_center : defocus_disk_sample(c);
  vec3_t ray_direction = vec3_sub(pixel_sample, ray_origin);

  return (ray_t){ray_origin, ray_direction};
}

color_t background_color(const ray_t r) {
  vec3_t  unit_direction = vec3_unit_vector(r.dir);
  double  t              = 0.5 * (unit_direction.y + 1.0);
  color_t white          = {1.0, 1.0, 1.0};
  color_t blue           = {0.5, 0.7, 1.0};
  return vec3_add(vec3_mul_sc(white, 1.0 - t), vec3_mul_sc(blue, t));
}

color_t ray_color(const ray_t r, const int max_depth, const world_t *world) {
  ray_t   current_ray         = r;         // Start with the input ray
  color_t current_attenuation = {1, 1, 1}; // Start with no attenuation

#ifdef __GNUC__
#pragma GCC unroll 10
#endif
  for (int depth = 0; depth < max_depth; depth++) {
    hit_record_t rec;

    if (!world_hit(world, &current_ray, 0.001, INFINITY, &rec)) {
      // If the ray doesn't hit anything, return the background color
      return vec3_mul(current_attenuation, background_color(current_ray));
    }

    ray_t   scattered;
    color_t attenuation;

    if (!mat_scatter(rec.mat, current_ray, rec, &attenuation, &scattered)) {
      return (color_t){0, 0, 0}; // Ray was absorbed
    }

    // Update the ray and attenuation
    current_attenuation = vec3_mul(current_attenuation, attenuation);
    current_ray         = scattered;
  }

  static color_t black = {0, 0, 0};
  // If we exceed the bounce limit, return black
  return black;
}

#define MIN(a, b) ((a) < (b) ? (a) : (b))

void camera_render(camera_t *c, const world_t *world, vec3_t *image) {
  camera_initialize(c);

  size_t img_size      = c->image_width * c->image_height;
  size_t total_samples = img_size * c->samples_per_pixel;
  int    num_threads   = omp_get_max_threads();

  RT_INFO("Rendering %zux%zu image with %d samples per pixel (%zu total samples)", c->image_width,
          c->image_height, c->samples_per_pixel, total_samples);

  RT_DEBUG("Using %d threads", num_threads);

  color_t *threaded_samples = malloc(sizeof(color_t) * img_size * num_threads);
  RT_ASSERT(threaded_samples != NULL, "Failed to allocate memory");

  double start_time = omp_get_wtime();
#pragma omp parallel default(none) num_threads(num_threads)                                        \
    shared(image, c, world, stderr, num_threads, threaded_samples, img_size, total_samples)
  {
    int rank = omp_get_thread_num();

    color_t *local_samples = threaded_samples + rank * img_size;

    // set every pixel to black
    for (size_t i = 0; i < img_size; i++) {
      local_samples[i] = (color_t){0, 0, 0};
    }

#pragma omp for collapse(3) schedule(dynamic, total_samples/100)
    for (size_t y = 0; y < c->image_height; y++) {
      for (size_t x = 0; x < c->image_width; x++) {
        for (int sample = 0; sample < c->samples_per_pixel; sample++) {
          ray_t   sample_ray   = camera_get_ray(c, x, y);
          color_t sample_color = ray_color(sample_ray, c->max_depth, world);

          size_t index         = y * c->image_width + x;
          local_samples[index] = vec3_add(local_samples[index], sample_color);
        }
      }
    }
  }

  double elapsed_time = omp_get_wtime() - start_time;
  RT_INFO("Rendering took %.0f ms (%.2f Msamples/s)", elapsed_time * 1000,
          (total_samples) / elapsed_time / 1e6);

  RT_INFO("Merging samples from %d threads", num_threads);

  start_time = omp_get_wtime();
  for (size_t j = 0; j < c->image_height; j++) {
    for (size_t i = 0; i < c->image_width; i++) {
      size_t img_index = j * c->image_width + i;

      for (int rank = 0; rank < num_threads; rank++) {
        size_t sample_index = rank * img_size + img_index;
        image[img_index]    = vec3_add(image[img_index], threaded_samples[sample_index]);
      }
      image[img_index] = vec3_mul_sc(image[img_index], c->_pixel_samples_scale);
    }
  }

  elapsed_time = omp_get_wtime() - start_time;
  RT_INFO("Merging samples took %.0f ms (%.2f Msamples/s)", elapsed_time * 1000,
          (total_samples) / elapsed_time / 1e6);

  free(threaded_samples);
}
