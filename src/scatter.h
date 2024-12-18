
#include "hit_record.h"
#include "material.h"

bool mat_scatter_dielectric(const mat_dielectric_t *d, const ray_t *r_in, hit_record_t rec,
                       color_t *attenuation, ray_t *scattered);
bool mat_scatter_lambertian(const mat_lambertian_t *l, hit_record_t rec, color_t *attenuation,
                       ray_t *scattered);
bool mat_scatter_metal(const mat_metal_t *m, const ray_t *r_in, hit_record_t rec,
                       color_t *attenuation, ray_t *scattered);
bool mat_scatter(mat_t m, ray_t r_in, hit_record_t rec, color_t *attenuation, ray_t *scattered);
