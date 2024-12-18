#include "world.h"
#include <stdlib.h>

void world_init(world_t *world) {
  world->objects     = NULL;
  world->num_objects = 0;
  world->capacity    = 0;
}

void _world_grow(world_t *world) {
  if (world->capacity == 0) {
    world->capacity = 1;
    world->objects  = malloc(world->capacity * sizeof(object_t));
  } else {
    world->capacity *= 2;
    world->objects = realloc(world->objects, world->capacity * sizeof(object_t));
  }
}

void world_add_object(world_t *world, object_t object) {
  if (world->num_objects == world->capacity) {
    _world_grow(world);
  }

  world->objects[world->num_objects++] = object;
}

void world_free(world_t *world) {
  free(world->objects);
  world->objects     = NULL;
  world->num_objects = 0;
  world->capacity    = 0;
}

bool world_hit(const world_t *world, const ray_t *ray, const double t_min, const double t_max,
               hit_record_t *rec) {
  hit_record_t temp_rec;
  bool         hit_anything   = false;
  double       closest_so_far = t_max;

  for (size_t i = 0; i < world->num_objects; i++) {
    object_t object = world->objects[i];
    switch (object.shape.type) {
    case SPHERE:
      if (sphere_hit(&object.shape.data.sphere, ray, t_min, closest_so_far, &temp_rec)) {
        hit_anything   = true;
        closest_so_far = temp_rec.t;
        *rec           = temp_rec;
        rec->mat       = object.mat;
      }
      break;

    case CUBE:
      if (cube_hit(&object.shape.data.cube, ray, t_min, closest_so_far, &temp_rec)) {
        hit_anything   = true;
        closest_so_far = temp_rec.t;
        *rec           = temp_rec;
        rec->mat       = object.mat;
      }
      break;
    }
  }

  return hit_anything;
}
