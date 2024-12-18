#include "scatter.h"
#include "hit_record.h"
#include "material.h"
#include "utils.h"

#include <math.h>

bool mat_scatter_lambertian(const mat_lambertian_t *l, const hit_record_t rec, color_t *attenuation,
                            ray_t *scattered) {
  vec3_t scatter_direction = vec3_add(rec.normal, vec3_rand_unit_disk());

  // Catch degenerate scatter direction
  if (vec3_near_zero(scatter_direction)) {
    scatter_direction = rec.normal;
  }

  *scattered   = (ray_t){rec.point, scatter_direction};
  *attenuation = l->albedo;
  return true;
}

bool mat_scatter_metal(const mat_metal_t *m, const ray_t *r_in, const hit_record_t rec,
                       color_t *attenuation, ray_t *scattered) {

  vec3_t reflected = vec3_reflect(r_in->dir, rec.normal);
  reflected = vec3_add(vec3_unit_vector(reflected), (vec3_mul_sc(vec3_rand_unit(), m->fuzz)));

  *scattered   = (ray_t){rec.point, reflected};
  *attenuation = m->albedo;

  return (vec3_dot(scattered->dir, rec.normal) > 0);
}

static inline double reflectance(const double cosine, const double refraction_index) {
  // Use Schlick's approximation for reflectance.
  double r0 = (1 - refraction_index) / (1 + refraction_index);
  r0        = r0 * r0;
  return r0 + (1 - r0) * pow(1 - cosine, 5);
}

bool mat_scatter_dielectric(const mat_dielectric_t *d, const ray_t *r_in, const hit_record_t rec,
                            color_t *attenuation, ray_t *scattered) {
  // Set attenuation to white (no absorption)
  *attenuation = (color_t){1.0, 1.0, 1.0};

  // Determine the refraction index ratio
  double refraction_ratio = rec.front_face ? (1.0 / d->refraction_index) : d->refraction_index;

  // Ensure the ray direction is a unit vector
  vec3_t unit_direction = vec3_unit_vector(r_in->dir);

  // Calculate cos(theta) where theta is the angle between -ray.direction and normal
  double cos_theta = fmin(vec3_dot(vec3_mul_sc(unit_direction, -1), rec.normal), 1.0);
  double sin_theta = sqrt(1.0 - cos_theta * cos_theta);

  // Determine if the ray cannot refract (total internal reflection)
  bool cannot_refract = refraction_ratio * sin_theta > 1.0;

  // Calculate the new ray direction: reflection or refraction
  vec3_t direction;
  if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_double()) {
    // Reflect the ray
    direction = vec3_reflect(unit_direction, rec.normal);
  } else {
    // Refract the ray
    direction = vec3_refract(unit_direction, rec.normal, refraction_ratio);
  }

  // Set the scattered ray
  *scattered = (ray_t){rec.point, direction};
  return true;
}

bool mat_scatter(const mat_t m, const ray_t r_in, const hit_record_t rec, color_t *attenuation,
                 ray_t *scattered) {
  switch (m.type) {
  case LAMBERTIAN:
    return mat_scatter_lambertian(&m.data.lambertian, rec, attenuation, scattered);
  case METAL:
    return mat_scatter_metal(&m.data.metal, &r_in, rec, attenuation, scattered);
  case DIELECTRIC:
    return mat_scatter_dielectric(&m.data.dielectric, &r_in, rec, attenuation, scattered);
  }

  RT_FATAL("Invalid material kind, %d", m.type);
}
