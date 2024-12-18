#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "camera.h"
#include "material.h"
#include "shapes.h"
#include "utils.h"
#include "vec3.h"
#include "world.h"

void write_color(FILE *out, vec3_t pixel_color) {
  double r = pixel_color.x;
  double g = pixel_color.y;
  double b = pixel_color.z;

  // Translate the [0,1] component values to the byte range [0,255].
  int rbyte = (int)(255.999 * r);
  int gbyte = (int)(255.999 * g);
  int bbyte = (int)(255.999 * b);

  fprintf(out, "%d %d %d\n", rbyte, gbyte, bbyte);
}

void write_image(char *output, vec3_t *image, int image_width, int image_height) {
  FILE *out_fd = fopen(output, "w");
  if (out_fd == NULL) {
    fprintf(stderr, "Error opening file %s: %s\n", output, strerror(errno));
    exit(EXIT_FAILURE);
  }

  fprintf(out_fd, "P3\n");
  fprintf(out_fd, "%d %d\n255\n", image_width, image_height);

  for (int j = 0; j < image_height; j++) {
    for (int i = 0; i < image_width; i++) {
      int index = j * image_width + i;
      write_color(out_fd, image[index]);
    }
  }

  /* chiude il file */
  fclose(out_fd);
}

color_t random_color() { return (color_t){random_double(), random_double(), random_double()}; }
color_t random_color_range(double min, double max) {
  return (color_t){random_double_range(min, max), random_double_range(min, max),
                   random_double_range(min, max)};
}

int main(int argc, char *argv[]) {

  int      samples_per_pixel;
  point3_t lookfrom = (point3_t){0, 2, 0};

  if (argc < 5) {
    fprintf(stderr,
            "Usage: %s <output> <image_width> <image_height> <samples_per_pixel>"
            "[lookfrom_x lookfrom_y lookfrom_z]\n",
            argv[0]);
    exit(EXIT_FAILURE);
  }

  char *output       = argv[1];
  int   image_width  = atoi(argv[2]);
  int   image_height = atoi(argv[3]);
  samples_per_pixel  = atoi(argv[4]);

  if (argc == 8) {
    lookfrom = (point3_t){atof(argv[5]), atof(argv[6]), atof(argv[7])};
  } else if (argc != 5) {
    fprintf(stderr, "Invalid number of arguments\n");
    exit(EXIT_FAILURE);
  }

  // Render

  world_t world;
  world_init(&world);

  for (int a = -11; a < 11; a += 2) {
    for (int b = -11; b < 11; b += 2) {
      double choose_mat = random_double();

      point3_t center = {a + 0.9 * random_double(), 0.2, b + 0.9 * random_double()};

      if (vec3_len(vec3_sub(center, (point3_t){4, 0.2, 0})) > 0.9) {
        mat_t sphere_material;

        if (choose_mat < 0.8) {
          // diffuse
          vec3_t albedo   = vec3_mul(random_color(), random_color());
          sphere_material = mat_lambertian(albedo);
          world_add_object(&world, object_new(shape_sphere(center, 0.2), sphere_material));
        } else if (choose_mat < 0.95) {
          // metal
          color_t albedo  = random_color_range(0.5, 1);
          double  fuzz    = random_double_range(0, 0.5);
          sphere_material = mat_metal(albedo, fuzz);
          world_add_object(&world, object_new(shape_sphere(center, 0.2), sphere_material));
        } else {
          // glass
          sphere_material = mat_dielectric(1.5);
          world_add_object(&world, object_new(shape_sphere(center, 0.2), sphere_material));
        }
      }
    }
  }

  mat_t   ground_material = mat_lambertian((vec3_t){0.5, 0.5, 0.5});
  shape_t ground_sphere   = shape_sphere((vec3_t){0, -5000, 0}, 5000);
  world_add_object(&world, object_new(ground_sphere, ground_material));

  mat_t   material1 = mat_metal((vec3_t){0.7, 0.6, 0.5}, 0.0);
  shape_t sphere1   = shape_sphere((vec3_t){-2, 1, 3}, 1.0);
  world_add_object(&world, object_new(sphere1, material1));

  mat_t   material2 = mat_dielectric(1.5);
  shape_t sphere2   = shape_sphere((vec3_t){-1, 1, 5}, 1.0);
  world_add_object(&world, object_new(sphere2, material2));

  mat_t   material3 = mat_lambertian((vec3_t){0.4, 0.2, 0.1});
  shape_t sphere3   = shape_sphere((vec3_t){0, 1, 6}, 1.0);
  world_add_object(&world, object_new(sphere3, material3));

  vec3_t *image = malloc(sizeof(vec3_t) * image_width * image_height);
  RT_ASSERT(image != NULL, "Failed to allocate memory for image");

  camera_t camera;
  camera_default(&camera);
  camera.image_width       = image_width;
  camera.image_height      = image_height;
  camera.samples_per_pixel = samples_per_pixel;
  camera.lookfrom          = lookfrom;
  camera.lookat            = (point3_t){0, 0, 5};

  camera_render(&camera, &world, image);
  world_free(&world);

  RT_INFO("Writing image to %s", output);
  write_image(output, image, image_width, image_height);

  free(image);
}
