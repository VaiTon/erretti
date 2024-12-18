#pragma once

#include <stddef.h>

#include "shapes.h"

typedef struct {
  shape_t shape;
  mat_t   mat;
} object_t;

inline static object_t object_new(shape_t shape, mat_t mat) {
  return (object_t){.shape = shape, .mat = mat};
}

typedef struct {
  object_t *objects;
  size_t    num_objects;
  size_t    capacity;
} world_t;

void world_init(world_t *world);
void world_add_object(world_t *world, object_t object);
void world_free(world_t *world);

bool world_hit(const world_t *world, const ray_t *ray, double t_min, double t_max,
               hit_record_t *rec);
