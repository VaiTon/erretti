#include <float.h>
#include <math.h>

#include "hit_record.h"
#include "ray.h"
#include "shapes.h"
#include "vec3.h"

bool sphere_hit(const sphere_t *sphere, const ray_t *ray, const double t_min, const double t_max,
                hit_record_t *rec) {
  vec3_t oc = vec3_sub(sphere->center, ray->orig);

  double a = vec3_sq(ray->dir);
  double half_b = vec3_dot(ray->dir, oc);
  double c = vec3_sq(oc) - sphere->radius * sphere->radius;

  double discriminant = half_b * half_b - a * c;
  if (discriminant < 0) {
    return false; // No intersection
  }

  double sqrtd = sqrt(discriminant);

  // Find the nearest root that lies in the acceptable range
  double root = (half_b - sqrtd) / a;
  if (root < t_min || root > t_max) {
    root = (half_b + sqrtd) / a;
    if (root < t_min || root > t_max) {
      return false;
    }
  }

  rec->t                = root;
  rec->point            = ray_at(*ray, rec->t);
  vec3_t outward_normal = vec3_div_sc(vec3_sub(rec->point, sphere->center), sphere->radius);
  hit_record_set_face_normal(rec, *ray, outward_normal);

  return true;
}

// Function to calculate tMin and tMax for one axis
bool intersect_axis(const double origin, const double direction, const double cube_min,
                    const double cube_max, double *t_min, double *t_max) {

  if (direction != 0.0f) {
    double t1 = (cube_min - origin) / direction;
    double t2 = (cube_max - origin) / direction;

    // Ensure t1 is the near intersection and t2 is the far intersection
    if (t1 > t2) {
      double temp = t1;
      t1          = t2;
      t2          = temp;
    }

    // Update tMin and tMax
    *t_min = t1 > *t_min ? t1 : *t_min;
    *t_max = t2 < *t_max ? t2 : *t_max;

    // If the intervals do not overlap, there is no intersection
    if (*t_min > *t_max) {
      return false; // No intersection
    }
  } else if (origin < cube_min || origin > cube_max) {
    return false; // Ray is parallel to the planes and outside the bounds
  }

  return true; // Intersection along this axis
}

bool cube_hit(const cube_t *cube, const ray_t *ray, const double t_min, const double t_max,
              hit_record_t *rec) {

  double t0 = t_min;
  double t1 = t_max;

  // Track which axis produces the near intersection

  // Unroll for X-axis
  if (!intersect_axis(ray->orig.x, ray->dir.x, cube->min.x, cube->max.x, &t0, &t1)) {
    return false;
  }
  int hit_axis = 0; // X-axis

  // Unroll for Y-axis
  if (!intersect_axis(ray->orig.y, ray->dir.y, cube->min.y, cube->max.y, &t0, &t1)) {
    return false;
  }
  if (t0 > t_min) {
    hit_axis = 1; // Y-axis
  }

  // Unroll for Z-axis
  if (!intersect_axis(ray->orig.z, ray->dir.z, cube->min.z, cube->max.z, &t0, &t1)) {
    return false;
  }
  if (t0 > t_min) {
    hit_axis = 2; // Z-axis
  }

  // Set the normal based on the axis that caused the hit
  vec3_t normal = {0, 0, 0};
  if (hit_axis == 0)
    normal.x = (ray->orig.x + t0 * ray->dir.x > cube->min.x) ? 1 : -1;
  if (hit_axis == 1)
    normal.y = (ray->orig.y + t0 * ray->dir.y > cube->min.y) ? 1 : -1;
  if (hit_axis == 2)
    normal.z = (ray->orig.z + t0 * ray->dir.z > cube->min.z) ? 1 : -1;

  rec->t     = t0;
  rec->point = ray_at(*ray, rec->t);
  hit_record_set_face_normal(rec, *ray, normal);

  return true;
}
