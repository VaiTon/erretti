#include "ray.h"
#include "vec3.h"

vec3_t ray_at(const ray_t r, const double dist) {
  return vec3_add(r.orig, vec3_mul_sc(r.dir, dist));
}
