#ifndef HITTABLE_H
#define HITTABLE_H

class material;

class hit_record {
    public:
        point3 p;                      // The point of intersection
        vec3 normal;                   // The normal at the intersection
        shared_ptr<material> mat;      // The material of the intersected object
        double t;                      // The parametric distance along the ray
        bool front_face;               // True if the hit surface faces the ray

        /**
         * Sets the normal to always face above the surface.
         * @param r The ray used for collision detection.
         * @param outward_normal The normal pointing outward from the surface.
         */
        void set_face_normal(const ray& r, const vec3& outward_normal) {
            // Check if the ray is coming from outside or inside
            // Decide the direction of the normal accordingly
            front_face = dot(r.direction(), outward_normal) < 0;
            normal = front_face ? outward_normal : -outward_normal;
        }
};

/**
 * An abstract class representing any object that can be hit by a ray.
 */
class hittable {
    public:
        virtual ~hittable() = default;

        /**
         * Checks if the ray intersects this object within the given interval.
         * @param r The ray.
         * @param ray_t The interval [min, max] for valid t values along the ray.
         * @param rec The record of the hit, filled if an intersection occurs.
         * @return True if there's an intersection, false otherwise.
         */
        virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const = 0;
};

#endif