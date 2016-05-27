#pragma once

#include "octreebuilder_api.h"
#include "build_options.h"

#include <initializer_list>
#include <iosfwd>

#ifdef OCTREEBUILDER_USE_SSE
#include <emmintrin.h>
typedef int coord_t;
#else
typedef long long coord_t;
#endif

typedef unsigned int uint;

namespace octreebuilder {

class OCTREEBUILDER_API Vector3i {
public:

    /**
     * @brief Creates vector (0,0,0)
     */
    Vector3i();

    /**
     * @brief Creates vector (x,y,z)
     */
    Vector3i(coord_t x, coord_t y, coord_t z);

    /**
     * @brief Creates vector (v,v,v)
     */
    explicit Vector3i(coord_t v);

    /**
     * @brief Creates a vector from an initializer list
     */
    Vector3i(::std::initializer_list<coord_t> init);

    bool operator==(const Vector3i& o) const;

    bool operator!=(const Vector3i& o) const;

    coord_t operator[](uint i) const;

    Vector3i operator-() const;

    /**
     * @brief Defines a strict weak ordering so that vectors can be used in a set
     */
    bool operator<(const Vector3i& o) const;

    Vector3i& operator+=(const Vector3i& o);

    Vector3i& operator-=(const Vector3i& o);

    Vector3i& operator*=(const coord_t& scalar);

    /**
     * @brief sets x,y and z to the maximum of this and the other vector
     */
    Vector3i& max(const Vector3i& o);

    /**
     * @brief sets x,y and z to the minimum of this and the other vector
     */
    Vector3i& min(const Vector3i& o);

    /**
     * @brief Returns the absolute of all components
     */
    Vector3i abs() const;

    coord_t dot(const Vector3i& o) const;

    coord_t x() const;
    void setX(coord_t value);

    coord_t y() const;
    void setY(coord_t value);

    coord_t z() const;
    void setZ(coord_t value);

private:
#ifdef OCTREEBUILDER_USE_SSE
    explicit Vector3i(__m128i data);
    __m128i m_data;
#else
    coord_t m_x;
    coord_t m_y;
    coord_t m_z;
#endif
};

OCTREEBUILDER_API Vector3i operator+(Vector3i a, const Vector3i& b);

OCTREEBUILDER_API Vector3i operator-(Vector3i a, const Vector3i& b);

OCTREEBUILDER_API Vector3i operator*(Vector3i a, coord_t scalar);

OCTREEBUILDER_API Vector3i operator*(coord_t scalar, Vector3i a);

/**
 * @brief Returns the vector with the maximum components of a and b
 * @return { max(a.x, b.x), max(a.y, b.y), max(a.z, b.z) }
 */
OCTREEBUILDER_API Vector3i max(Vector3i a, const Vector3i& b);

/**
 * @brief Returns the vector with the minimum components of a and b
 * @return { min(a.x, b.x), min(a.y, b.y), min(a.z, b.z) }
 */
OCTREEBUILDER_API Vector3i min(Vector3i a, const Vector3i& b);

OCTREEBUILDER_API::std::ostream& operator<<(::std::ostream& s, const Vector3i& c);
}
