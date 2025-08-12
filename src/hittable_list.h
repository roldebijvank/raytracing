#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "hittable.h"
#include <vector>

class hittable_list : public hittable {
    public:
        // The container of scene objects
        std::vector<shared_ptr<hittable>> objects;

        hittable_list() {}
        hittable_list(shared_ptr<hittable> object) { add(object); }

        // Remove all objects from the list
        void clear() { objects.clear(); }

        // Add an object to the list
        void add(shared_ptr<hittable> object) {
            objects.push_back(object);
        }

        /**
         * Check for hits against each object in the list, returning true if the ray hits something.
         * Keeps track of the closest hit to the ray origin.
         * @param r The ray to test for intersection.
         * @param ray_t The interval defining the valid range of t for the ray.
         * @param rec Records hit information such as t, hit point, and normal.
         * @return True if the ray hits any object, false otherwise.
         */
        bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
            hit_record temp_rec;
            bool hit_anything = false;
            auto closest_so_far = ray_t.max;

            // Check every object and keep track of the closest intersection
            for (const auto& object : objects) {
                if (object->hit(r, interval(ray_t.min, closest_so_far), temp_rec)) {
                    hit_anything = true;
                    closest_so_far = temp_rec.t;  // Update the closest t-value
                    rec = temp_rec;               // Save the new closest hit record
                }
            }

            return hit_anything;
        }
};

#endif