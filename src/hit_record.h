#pragma once

#include "material.h"
#include "ray.h"
#include "vec3.h"

typedef struct {
  mat_t    mat;        // Hit material
  point3_t point;      // Point of intersection
  vec3_t   normal;     // Normal at the intersection point
  double   t;          // Distance from the ray origin
  bool     front_face; // True if the normal is facing the ray
} hit_record_t;

// Set the face normal of the hit record
void hit_record_set_face_normal(hit_record_t *rec, ray_t r, vec3_t outward_normal);
