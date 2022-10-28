
#ifndef MATERIAL_H
#define MATERIAL_H

#include "rtweekend.h"
#include "hitable.h"
#include "texture.h"
struct hit_record;

class material {
    public:
        virtual bool scatter(
            const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
        ) const = 0;
};


class lambertian : public material {
    public:
        lambertian(const color& a) : albedo(make_shared<solid_color>(a)) {}
        lambertian(shared_ptr<texture> a) : albedo(a) {}

        virtual bool scatter(
            const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
        ) const override {
            auto scatter_direction = rec.normal + random_unit_vector();

            // Catch degenerate scatter direction
            if (scatter_direction.near_zero())
                scatter_direction = rec.normal;

            scattered = ray(rec.p, scatter_direction, r_in.time());
            attenuation = albedo->value(rec.u, rec.v, rec.p);
            return true;
        }

    public:
       shared_ptr<texture> albedo;
};

class metal : public material {
    public:
        metal(const color& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

        virtual bool scatter(
            const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
        ) const override {
            vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
            scattered = ray(rec.p, reflected + fuzz*random_in_unit_sphere(), r_in.time());
            attenuation = albedo;
            return (dot(scattered.direction(), rec.normal) > 0);
        }

    public:
        color albedo;
        double fuzz;
};

class dielectric : public material {
public:
 dielectric(float ri) : ref_idx(ri) {}
 virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const {
     vec3 outward_normal;
     vec3 reflected = reflect(r_in.direction(), rec.normal);
     float ni_over_nt;
     attenuation = vec3(1.0,1.0,1.0);
     vec3 refracted;
     float reflect_prob;
     float cosine;
     if(dot(r_in.direction(), rec.normal) > 0) {
         outward_normal = -rec.normal;
         ni_over_nt = ref_idx;
         cosine = dot(r_in.direction(), rec.normal) / r_in.direction().length();
         cosine = sqrt(1 - ref_idx*ref_idx*(1-cosine*cosine));
     } else {
         outward_normal = rec.normal;
         ni_over_nt = 1.0 / ref_idx;
         cosine = -dot(r_in.direction(),rec.normal) / r_in.direction().length();
     }
     if(refract(r_in.direction(), outward_normal, ni_over_nt, refracted)) {
         reflect_prob = schlick(cosine,ref_idx);
     } else {
         reflect_prob = 1.0;
     }
     if (drand48() < reflect_prob) {
         scattered = ray(rec.p,reflected,r_in.time());
     } else {
         scattered = ray(rec.p,refracted,r_in.time());
     }
     return true;
 }
 float ref_idx;
    private:
            static double reflectance(double cosine, double ref_idx) {
                // Use Schlick's approximation for reflectance.
                auto r0 = (1-ref_idx) / (1+ref_idx);
                r0 = r0*r0;
                return r0 + (1-r0)*pow((1 - cosine),5);
            }
};
#endif
