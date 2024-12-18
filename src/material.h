#pragma once

#include "ray.h"
#include "utils.h"
#include "vec3.h"

typedef enum {
  LAMBERTIAN,
  METAL,
  DIELECTRIC,
} mat_type_t;

typedef struct {
  color_t albedo;
} mat_lambertian_t;

typedef struct {
  color_t albedo;
  double  fuzz;
} mat_metal_t;

typedef struct {
  double refraction_index;
} mat_dielectric_t;

typedef union {
  mat_lambertian_t lambertian;
  mat_metal_t      metal;
  mat_dielectric_t dielectric;
} mat_data_t;

typedef struct {
  mat_type_t type;
  mat_data_t data;
} mat_t;

inline static mat_t mat_lambertian(color_t albedo) {
  return (mat_t){
      .type = LAMBERTIAN,
      .data = {.lambertian = {.albedo = albedo}},
  };
}

inline static mat_t mat_metal(color_t albedo, double fuzz) {
  return (mat_t){
      .type = METAL,
      .data = {.metal = {.albedo = albedo, .fuzz = fuzz}},
  };
}

inline static mat_t mat_dielectric(double refraction_index) {
  return (mat_t){
      .type = DIELECTRIC,
      .data = {.dielectric = {.refraction_index = refraction_index}},
  };
}
