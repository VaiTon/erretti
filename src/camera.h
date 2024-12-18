#pragma once

#include "utils.h"
#include "vec3.h"
#include "world.h"

typedef struct {
  size_t image_width;  // Image width in pixels
  size_t image_height; // Image height in pixels

  double aspect_ratio; // Ratio of image width over height

  int samples_per_pixel; // Count of random samples for each pixel
  int max_depth;         // Maximum number of ray bounces into scene

  double   vfov;     // Vertical view angle (field of view)
  point3_t lookfrom; // Point camera is looking from
  point3_t lookat;   // Point camera is looking at
  vec3_t   vup;      // Camera-relative "up" direction

  double defocus_angle; // Variation angle of rays through each pixel
  double focus_dist;    // Distance from camera lookfrom point to plane of perfect focus

  double   _pixel_samples_scale; // Color scale factor for a sum of pixel samples
  point3_t _center;              // Camera center
  point3_t _pixel00_loc;         // Location of pixel 0, 0
  vec3_t   _pixel_delta_u;       // Offset to pixel to the right
  vec3_t   _pixel_delta_v;       // Offset to pixel below
  vec3_t   _u, _v, _w;           // Camera frame basis vectors
  vec3_t   _defocus_disk_u;      // Defocus disk horizontal radius
  vec3_t   _defocus_disk_v;      // Defocus disk vertical radius
} camera_t;

void camera_default(camera_t *c);
void camera_initialize(camera_t *c);
void camera_render(camera_t *c, const world_t *world, vec3_t *image);
