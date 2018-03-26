#ifndef DCDRAW_VECTOR_H
#define DCDRAW_VECTOR_H

namespace DCDraw {
    class Vector {
    public:
        float x, y;

        Vector() : x(0.0f), y(0.0f) {}

        Vector(float x, float y) : x(x), y(y) {}

        /// Return one of the vector elements array-style
        float operator[](int i) const {
            return i ? y : x;
        }

        /// Compare two vectors for equality
        bool operator==(const Vector &other) const {
            return x == other.x && y == other.y;
        }

        /// Compare two vectors for inequality
        bool operator!=(const Vector &other) const {
            return x != other.x || y != other.y;
        }

        /// Add two vectors
        Vector operator+(const Vector &other) const {
            return {x + other.x, y + other.y};
        }

        /// Subtract two vectors
        Vector operator-(const Vector &other) const {
            return {x - other.x, y - other.y};
        }

        /// Multiply by a scalar
        Vector operator*(float s) const {
            return {x * s, y * s};
        }

        /// Inline add two vectors
        Vector &operator+=(const Vector &other) {
            x += other.x;
            y += other.y;
            return *this;
        }

        /// Inline subtract two vectors
        Vector &operator-=(const Vector &other) {
            x -= other.x;
            y -= other.y;
            return *this;
        }

        // Inline multiply by a scalar
        Vector &operator*=(float s) {
            x *= s;
            y *= s;
            return *this;
        }
    };
}

#endif

