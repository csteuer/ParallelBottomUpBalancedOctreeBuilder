#pragma once

#include "octreebuilder_api.h"

#include <initializer_list>
#include <iosfwd>

typedef long long coord_t;
typedef unsigned int uint;

namespace octreebuilder {

class OCTREEBUILDER_API Vector3i {
public:
    /**
     * @brief Creates vector (0,0,0)
     */
    constexpr Vector3i() : m_x(0), m_y(0), m_z(0) {
    }

    /**
     * @brief Creates vector (x,y,z)
     */
    constexpr Vector3i(coord_t x, coord_t y, coord_t z) : m_x(x), m_y(y), m_z(z) {
    }

    /**
     * @brief Creates vector (v,v,v)
     */
    explicit constexpr Vector3i(coord_t v) : m_x(v), m_y(v), m_z(v) {
    }

    /**
     * @brief Creates a vector from an initializer list
     */
    Vector3i(::std::initializer_list<coord_t> init);

    bool operator==(const Vector3i& o) const;

    bool operator!=(const Vector3i& o) const;

    coord_t operator[](uint i) const;

    coord_t& operator[](uint i);

    Vector3i operator-() const;

    /**
     * @brief Defines a strict weak ordering so that vectors can be used in a set
     */
    bool operator<(const Vector3i& o) const;

    /**
     * @brief Returns the absolute of all components
     */
    Vector3i abs() const;

    coord_t dot(const Vector3i& other) const;

    coord_t x() const;
    void setX(coord_t value);

    coord_t y() const;
    void setY(coord_t value);

    coord_t z() const;
    void setZ(coord_t value);

private:
    coord_t m_x;
    coord_t m_y;
    coord_t m_z;
};

OCTREEBUILDER_API Vector3i operator+(const Vector3i& a, const Vector3i& b);

OCTREEBUILDER_API Vector3i operator-(const Vector3i& a, const Vector3i& b);

OCTREEBUILDER_API Vector3i operator*(const Vector3i& c, const coord_t& scalar);

OCTREEBUILDER_API Vector3i operator*(const coord_t& scalar, const Vector3i& c);

/**
 * @brief Returns the vector with the maximum components of a and b
 * @return { max(a.x, b.x), max(a.y, b.y), max(a.z, b.z) }
 */
OCTREEBUILDER_API Vector3i max(const Vector3i& a, const Vector3i& b);

/**
 * @brief Returns the vector with the minimum components of a and b
 * @return { min(a.x, b.x), min(a.y, b.y), min(a.z, b.z) }
 */
OCTREEBUILDER_API Vector3i min(const Vector3i& a, const Vector3i& b);

OCTREEBUILDER_API ::std::ostream& operator<<(::std::ostream& s, const Vector3i& c);
}
