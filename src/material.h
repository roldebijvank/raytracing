#ifndef MATERIAL_H
#define MATERIAL_H

#include "hittable.h"

/**
 * Abstract base class for materials. 
 * Defines the interface for scattering rays upon hitting an object.
 */
class material {
    public:
        virtual ~material() = default;

        /**
         * Scatter the incoming ray and determine the attenuation and scattered ray.
         * @param r_in The incoming ray.
         * @param rec The hit record containing intersection details.
         * @param attenuation The color attenuation (reduction in intensity).
         * @param scattered The scattered ray.
         * @return True if the ray is scattered, false otherwise.
         */
        virtual bool scatter (
            const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
        ) const {
            return false;
        }
};

/**
 * Lambertian (diffuse) material.
 * Scatters rays in random directions.
 */
class lambertian : public material {
    public:
        lambertian(const color& albedo) : albedo(albedo) {}

        bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
            auto scatter_direction = rec.normal + random_unit_vector();

            // Catch degenerate scatter direction
            if (scatter_direction.near_zero()) {
                scatter_direction = rec.normal;
            }

            scattered = ray(rec.p, scatter_direction);
            attenuation = albedo;
            return true;
        }
    
    private:
        color albedo;  // The base color of the material
};

/**
 * Metal material.
 * Reflects rays with some fuzziness.
 */
class metal : public material {
    public:
        metal(const color& albedo, double fuzz) : albedo(albedo), fuzz(fuzz < 1 ? fuzz : 1) {}

        bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
            vec3 reflected = reflect(r_in.direction(), rec.normal);
            reflected = unit_vector(reflected) + (fuzz * random_unit_vector());
            scattered = ray(rec.p, reflected);
            attenuation = albedo;
            return (dot(scattered.direction(), rec.normal) > 0);
        }

    private:
        color albedo;  // The base color of the material
        double fuzz;   // The fuzziness factor (0 = perfect reflection, 1 = maximum fuzziness)
};

/**
 * Dielectric (transparent) material.
 * Handles refraction and reflection based on the material's refractive index.
 */
class dielectric : public material {
    public:
        dielectric(double refraction_index) : refraction_index(refraction_index) {}

        bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
            attenuation = color(1.0, 1.0, 1.0);  // No attenuation for dielectric materials
            double ri = rec.front_face ? (1.0 / refraction_index) : refraction_index;

            vec3 unit_direction = unit_vector(r_in.direction());

            auto cos_theta = std::fmin(dot(-unit_direction, rec.normal), 1.0);
            auto sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);

            bool cannot_refract = ri * sin_theta > 1.0;
            vec3 direction;

            // Determine whether to reflect or refract based on the angle and refractive index
            if (cannot_refract || reflectance(cos_theta, ri) > random_double()) {
                direction = reflect(unit_direction, rec.normal);
            } else {
                direction = refract(unit_direction, rec.normal, ri);
            }

            scattered = ray(rec.p, direction);
            return true;
        }

    private:
        double refraction_index;  // The refractive index of the material

        /**
         * Compute the reflectance using Schlick's approximation.
         * @param cosine The cosine of the angle of incidence.
         * @param refraction_index The refractive index.
         * @return The reflectance value.
         */
        static double reflectance(double cosine, double refraction_index) {
            // Schlick's approximation for reflectance
            auto r0 = (1 - refraction_index) / (1 + refraction_index);
            r0 = r0 * r0;
            return r0 + (1 - r0) * std::pow((1 - cosine), 5);
        }
};

#endif