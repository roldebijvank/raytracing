#ifndef INTERVAL_H
#define INTERVAL_H

/**
 * Represents a range [min, max] in one dimension.
 * Provides methods for checking whether a value is inside or outside,
 * as well as clamping values to remain within the interval.
 */
class interval {
    public:
        double min;     // The lower bound of the interval
        double max;     // The upper bound of the interval

        /**
         * Default constructor: Creates an empty interval.
         * min is set to +∞, max is set to -∞, so size() is negative.
         */
        interval() : min(+infinity), max(-infinity) {}

        /**
         * Construct an interval with given bounds [min, max].
         */
        interval(double min, double max) : min(min), max(max) {}

        /**
         * @return The size of the interval, i.e., max - min.
         */
        double size() const {
            return max - min;
        }

        /**
         * Check if a value x is within the closed interval [min, max].
         */
        bool contains(double x) const {
            return min <= x && x <= max;
        }

        /**
         * Check if a value x is strictly within (min, max).
         */
        bool surrounds(double x) const {
            return min < x && x < max;
        }

        /**
         * Clamps a value x to ensure it is within [min, max].
         * If x is below min, returns min; if x is above max, returns max.
         * Otherwise returns x unchanged.
         */
        double clamp(double x) const {
            if (x < min) return min;
            if (x > max) return max;
            return x;
        }

        // Common global intervals
        static const interval empty;     // An empty interval (no valid range)
        static const interval universe;  // The entire real number range
};

// Define the static members
const interval interval::empty = interval(+infinity, -infinity);
const interval interval::universe = interval(-infinity, +infinity);

#endif