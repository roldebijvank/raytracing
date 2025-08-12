#ifndef COLOR_H
#define COLOR_H

#include "interval.h"
#include "vec3.h"

using color = vec3;

/**
 * Convert the color from linear space to gamma space.
 * Since displays and humans don't perceive brightness linearly,
 * this function applies a gamma correction.
 * @param linear_component The linear value of the color component.
 * @return The gamma-corrected value, which is the square root of the input or 0 if input <= 0.
 */
inline double linear_to_gamma(double linear_component) {
    if (linear_component > 0) {
        return std::sqrt(linear_component);
    }

    return 0;
}

/**
 * Write the color to an output stream in a format suitable for PPM images.
 * This function converts the color from linear space to gamma space,
 * clamps the values to the range [0, 0.999], scales them to the range [0, 255],
 * and writes them to the output stream.
 * @param out The output stream to write the color to.
 * @param pixel_color The color to write, represented as a vec3.
 */
void write_color(std::ostream& out, const color& pixel_color) {
    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();

    r = linear_to_gamma(r);
    g = linear_to_gamma(g);
    b = linear_to_gamma(b);


    // Translate the [0,1] component values to the byte range [0,255].
    static const interval intensity(0.000, 0.999);
    int rbyte = int(256 * intensity.clamp(r));
    int gbyte = int(256 * intensity.clamp(g));
    int bbyte = int(256 * intensity.clamp(b));

    // Write out the pixel color components.
    out << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';
}

#endif