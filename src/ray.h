#pragma once

#include "vec3.h"

typedef struct {
  point3_t orig;
  vec3_t   dir;
} ray_t;

point3_t ray_at(ray_t r, double t);
