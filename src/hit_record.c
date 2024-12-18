#include "hit_record.h"
#include "ray.h"

void hit_record_set_face_normal(hit_record_t *rec, const ray_t r, const vec3_t outward_normal) {
  rec->front_face = vec3_dot(r.dir, outward_normal) < 0;
  rec->normal     = rec->front_face ? outward_normal : vec3_mul_sc(outward_normal, -1);
}
