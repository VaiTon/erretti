#pragma once

#include "hit_record.h"
#include "ray.h"
#include "vec3.h"

typedef struct {
  point3_t center;
  double   radius;
} sphere_t;

bool sphere_hit(const sphere_t *sphere, const ray_t *ray, double t_min, double t_max,
                hit_record_t *rec);

typedef struct {
  point3_t min;
  point3_t max;
} cube_t;

bool cube_hit(const cube_t *cube, const ray_t *ray, double t_min, double t_max, hit_record_t *rec);

typedef enum { SPHERE, CUBE } shape_type_t;

typedef union {
  sphere_t sphere;
  cube_t   cube;
} shape_data_t;

typedef struct {
  shape_type_t type;
  shape_data_t data;
} shape_t;

inline static shape_t shape_sphere(point3_t center, double radius) {
  return (shape_t){
      .type = SPHERE,
      .data = {.sphere = {.center = center, .radius = radius}},
  };
}

inline static shape_t shape_cube(point3_t min, point3_t max) {
  return (shape_t){
      .type = CUBE,
      .data = {.cube = {.min = min, .max = max}},
  };
}
