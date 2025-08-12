#ifndef CAMERA_H
#define CAMERA_H

#include "hittable.h"
#include "material.h"

class camera {
    public:
        // image
        double aspect_ratio     = 1.0;      // ratio of image width over height
        int image_width         = 100;      // rendered image width in pixels
        int samples_per_pixel   = 10;       // count of random samples for each pixel
        int max_depth           = 10;       // maximum number of ray bounces into scene

        double vfov = 90;                   // vertical field of view
        point3 lookfrom = point3(0, 0, 0);  // Point camera is looking from
        point3 lookat   = point3(0, 0, -1); // Point camera is looking at
        vec3 vup        = vec3(0, 1, 0);    // Up vector for camera orientation

        double defocus_angle = 0;           // Variation angle of rays through each pixel
        double focus_dist = 10;             // Distance from camera lookfrom point to plane of perfect focus

        /**
         * Render the scene by casting rays into the world.
         * @param world The scene to render.
         */
        void render(const hittable& world) {
            initialize();

            // Output the PPM header
            std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

            // Loop over each pixel in the image
            for (int j = 0; j < image_height; j++) {
                std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
                for (int i = 0; i < image_width; i++) {
                    color pixel_color(0,0,0);
                    
                    // Accumulate color samples for anti-aliasing
                    for (int sample = 0; sample < samples_per_pixel; sample++) {
                        ray r = get_ray(i, j);
                        pixel_color += ray_color(r, max_depth, world);
                    }

                    // Write the accumulated color to the output stream
                    write_color(std::cout, pixel_color * pixel_samples_scale);
                }
            }
            std::clog << "\rDone.                  \n";
        }

    private:
        int     image_height;         // rendered image height
        double  pixel_samples_scale;  // color scale factor for a sum of pixel samples
        point3  center;               // camera center
        point3  pixel00_loc;          // location of pixel 0, 0
        vec3    pixel_delta_u;        // offset of pixel to the right
        vec3    pixel_delta_v;        // offset of pixel below
        vec3    u, v, w;              // camera basis vectors
        vec3    defocus_disk_u;       // Defocus disk horizontal radius
        vec3    defocus_disk_v;       // Defocus disk vertical radius

        /**
         * Initialize the camera parameters and precompute values for rendering.
         */
        void initialize() {
            // calculate image height, must be at least one
            image_height = int(image_width / aspect_ratio);
            image_height = (image_height < 1) ? 1 : image_height;

            pixel_samples_scale = 1.0 / samples_per_pixel;

            center = lookfrom;

            // camera
            auto theta = degrees_to_radians(vfov);
            auto h = std::tan(theta/2);
            auto viewport_height = 2 * h * focus_dist;
            auto viewport_width = viewport_height * (double(image_width) / image_height);

            // calculate camera basis vectors
            w = unit_vector(lookfrom - lookat);
            u = unit_vector(cross(vup, w));
            v = cross(w, u);

            // calculate vectors across horizontal and vertical edges of viewport
            vec3 viewport_u = u * viewport_width;       // horizontal
            vec3 viewport_v = -v * viewport_height;      // vertical

            // calculate horizontal and vertical delta vectors from pixel to pixel
            pixel_delta_u = viewport_u / image_width;
            pixel_delta_v = viewport_v / image_height;

            // calculate location of upper left pixel
            auto viewport_upper_left = center - (focus_dist * w) - viewport_u/2 - viewport_v/2;
            pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

            // Calculate the camera defocus disk basis vectors.
            auto defocus_radius = focus_dist * std::tan(degrees_to_radians(defocus_angle / 2));
            defocus_disk_u = u * defocus_radius;
            defocus_disk_v = v * defocus_radius;
        }

        /**
         * Generate a ray from the camera through the pixel at (i, j).
         * @param i The horizontal pixel index.
         * @param j The vertical pixel index.
         * @return The ray passing through the pixel.
         */
        ray get_ray(int i, int j) const {
            // Construct a camera ray originating from the origin and directed at randomly sampled
            // point around the pixel location i, j

            auto offset = sample_square();
            auto pixel_sample = pixel00_loc
                                + ((i + offset.x()) * pixel_delta_u)
                                + ((j + offset.y()) * pixel_delta_v);

            auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
            auto ray_direction = pixel_sample - ray_origin;

            return ray(ray_origin, ray_direction);
        }

        /**
         * Sample a random point within the unit square [-0.5, -0.5] to [0.5, 0.5].
         * @return A vector to a random point in the unit square.
         */
        vec3 sample_square() const {
            // returns vector to a random point in the [-0.5, -0.5] - [0.5, 0.5] unit square
            return vec3(random_double() - 0.5, random_double() - 0.5, 0);
        }

        point3 defocus_disk_sample() const {
            // Returns a random point in the camera defocus disk.
            auto p = random_in_unit_disk();
            return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
        }

        /**
         * Compute the color seen along a ray.
         * @param r The ray to trace.
         * @param depth The maximum recursion depth for ray bounces.
         * @param world The scene to render.
         * @return The color seen along the ray.
         */
        color ray_color(const ray& r, int depth, const hittable& world) const {
            // If we've exceeded the ray bounce limit, no more light is gathered
            if (depth <= 0) {
                return color(0, 0, 0);
            }

            hit_record rec;

            // Check if the ray hits anything in the world
            // The 0.001 minimum is to avoid "shadow acne" from numerical precision issues
            if (world.hit(r, interval(0.0001, infinity), rec)) {
                ray scattered;          // Will hold the scattered ray direction
                color attenuation;      // Will hold how much the light is attenuated by this hit
                
                // Use the material to determine how the ray is scattered
                if (rec.mat->scatter(r, rec, attenuation, scattered)) {
                    // Recursively trace the scattered ray and multiply by attenuation
                    return attenuation * ray_color(scattered, depth-1, world);
                }
                
                // If material absorbed all light (didn't scatter), return black
                return color(0, 0, 0);
            }

            // If ray hits nothing, return background color (sky gradient)
            vec3 unit_direction = unit_vector(r.direction());
            auto a = 0.5 * (unit_direction.y() + 1.0);    // Map y from [-1,1] to [0,1] for gradient
            return (1.0-a)*color(1.0, 1.0, 1.0) + a*color(0.5, 0.7, 1.0);  // Blend white and blue
        }
};

#endif