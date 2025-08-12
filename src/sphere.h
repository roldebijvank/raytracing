#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"

/**
 * Represents a sphere in 3D space that can be hit by rays.
 */
class sphere : public hittable {
    public:
        /**
         * Constructor to initialize the sphere with a center, radius, and material.
         * @param center The center of the sphere.
         * @param radius The radius of the sphere.
         * @param mat The material of the sphere.
         */
        sphere(const point3& center, double radius, shared_ptr<material> mat)
            : center(center), radius(std::fmax(0, radius)), mat(mat) {}

        /**
         * Check if the ray hits the sphere within the given interval.
         * @param r The ray to test for intersection.
         * @param ray_t The interval defining the valid range of t for the ray.
         * @param rec Records hit information such as t, hit point, and normal.
         * @return True if the ray hits the sphere, false otherwise.
         */
        bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
            vec3 oc = center - r.origin();                  // Vector from the ray origin to the sphere center
            auto a = r.direction().length_squared();        // Squared length of the ray direction
            auto h = dot(r.direction(), oc);                // Projection of oc onto the ray direction
            auto c = oc.length_squared() - radius*radius;   // Squared length of vector from the ray origin to sphere center

            auto discriminant = h*h - a*c;

            // If the discriminant is negative, the ray does not intersect the sphere
            if (discriminant < 0) {
                return false;
            }

            auto sqrtd = std::sqrt(discriminant);

            // Find the nearest root that lies in the acceptable range
            auto root = (h - sqrtd) / a;
            if (!ray_t.contains(root)) {
                root = (h + sqrtd) / a;
                if (!ray_t.contains(root)) {
                    return false;
                }
            }

            // Record the hit information
            rec.t = root;
            rec.p = r.at(rec.t);
            vec3 outward_normal = (rec.p - center) / radius;
            rec.set_face_normal(r, outward_normal);
            rec.mat = mat;

            return true;
        }

    private:
        point3 center;                // The center of the sphere
        double radius;                // The radius of the sphere
        shared_ptr<material> mat;     // The material of the sphere
};

#endif