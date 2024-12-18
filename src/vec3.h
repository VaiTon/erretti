#pragma once

#include "utils.h"
#include <math.h>
#include <stdbool.h>

typedef struct {
  double x, y, z;
} vec3_t;

typedef vec3_t point3_t;
typedef vec3_t color_t;

inline static vec3_t   vec3_new(double x, double y, double z) { return (vec3_t){x, y, z}; }
inline static color_t  color_new(double r, double g, double b) { return vec3_new(r, g, b); }
inline static point3_t point3_new(double x, double y, double z) { return vec3_new(x, y, z); }

inline static vec3_t vec3_zero() { return vec3_new(0, 0, 0); }

inline static vec3_t vec3_neg(const vec3_t vec) { return vec3_new(-vec.x, -vec.y, -vec.z); }

inline static double vec3_dot(const vec3_t vec1, const vec3_t vec2) {
  return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;
}

inline static vec3_t vec3_add(const vec3_t vec1, const vec3_t vec2) {
  return vec3_new(vec1.x + vec2.x, vec1.y + vec2.y, vec1.z + vec2.z);
}

inline static vec3_t vec3_add_sc(const vec3_t vec, const double scalar) {
  return vec3_new(vec.x + scalar, vec.y + scalar, vec.z + scalar);
}

inline static vec3_t vec3_sub(const vec3_t vec1, const vec3_t vec2) {
  return vec3_new(vec1.x - vec2.x, vec1.y - vec2.y, vec1.z - vec2.z);
}

inline static vec3_t vec3_mul(const vec3_t vec1, const vec3_t vec2) {
  return vec3_new(vec1.x * vec2.x, vec1.y * vec2.y, vec1.z * vec2.z);
}

inline static vec3_t vec3_mul_sc(const vec3_t vec, const double scalar) {
  return vec3_new(vec.x * scalar, vec.y * scalar, vec.z * scalar);
}

inline static vec3_t vec3_div_sc(const vec3_t vec1, const double vec2) {
  return vec3_mul_sc(vec1, 1.0 / vec2);
}

inline static double vec3_len_sq(const vec3_t vec) {
  return vec.x * vec.x + vec.y * vec.y + vec.z * vec.z;
}

inline static double vec3_len(const vec3_t vec) { return sqrt(vec3_len_sq(vec)); }

inline static bool vec3_near_zero(const vec3_t v) {
  const double s = 1e-8; // Soglia di tolleranza
  return fabs(v.x) < s && fabs(v.y) < s && fabs(v.z) < s;
}

inline static vec3_t vec3_rand() {
  return vec3_new(random_double(), random_double(), random_double());
}

inline static vec3_t vec3_rand_range(const double min, const double max) {
  const vec3_t result = {
      random_double_range(min, max),
      random_double_range(min, max),
      random_double_range(min, max),
  };
  return result;
}

inline static vec3_t vec3_cross(const vec3_t vec1, const vec3_t vec2) {
  return vec3_new(vec1.y * vec2.z - vec1.z * vec2.y, vec1.z * vec2.x - vec1.x * vec2.z,
                  vec1.x * vec2.y - vec1.y * vec2.x);
}

inline static vec3_t vec3_unit_vector(const vec3_t vec) { return vec3_div_sc(vec, vec3_len(vec)); }

inline static vec3_t vec3_rand_unit_disk() {
  vec3_t p;
  do {
    p = vec3_new(random_double_range(-1, 1), random_double_range(-1, 1), 0);
  } while (vec3_len_sq(p) >= 1);

  return p;
}

inline static vec3_t vec3_rand_unit() {
  vec3_t p;
  double lensq;
  do {
    p     = vec3_rand_range(-1, 1);
    lensq = vec3_len_sq(p);
  } while (lensq <= 1e-160 || lensq > 1.0);

  return vec3_div_sc(p, sqrt(lensq));
}

inline static vec3_t vec3_rand_hemisphere(const vec3_t normal) {
  const vec3_t in_unit_sphere = vec3_rand_unit();

  if (vec3_dot(in_unit_sphere, normal) > 0.0) {
    return in_unit_sphere; // Nello stesso emisfero del normale
  }

  return vec3_neg(in_unit_sphere);
}

inline static vec3_t vec3_reflect(const vec3_t vec, const vec3_t norm) {
  double dot_product = 2 * vec3_dot(vec, norm);
  vec3_t scaled_n    = vec3_mul_sc(norm, dot_product);
  vec3_t result      = vec3_sub(vec, scaled_n);
  return result;
}

inline static vec3_t vec3_refract(const vec3_t vec, const vec3_t norm,
                                  const double etai_over_etat) {
  double cos_theta      = fmin(vec3_dot(vec3_mul_sc(vec, -1), norm), 1.0);
  vec3_t r_out_perp     = vec3_mul_sc(vec3_add(vec, vec3_mul_sc(norm, cos_theta)), etai_over_etat);
  // F// = sqrt(1 - |r_out_perp|^2)
  vec3_t r_out_parallel = vec3_mul_sc(norm, -sqrt(fabs(1.0 - vec3_len_sq(r_out_perp))));
  return vec3_add(r_out_perp, r_out_parallel);
}

inline static double vec3_sq(const vec3_t vec) { return vec3_dot(vec, vec); }
