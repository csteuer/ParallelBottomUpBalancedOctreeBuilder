#include "vector3i.h"

#include <stdexcept>
#include <cmath>
#include <ostream>

#ifdef OCTREEBUILDER_USE_SSE
#include <smmintrin.h>
#endif

namespace octreebuilder {

#ifdef OCTREEBUILDER_USE_SSE
Vector3i::Vector3i() : m_data(_mm_setzero_si128()) {
}

Vector3i::Vector3i(coord_t x, coord_t y, coord_t z) : m_data(_mm_setr_epi32(x, y, z, 0)) {
}

Vector3i::Vector3i(coord_t v)
    : m_data(_mm_setr_epi32(v, v, v, 0)) {  // do not use _mm_set1_epi32 since 4th component must be zero... otherwise equality operator won't work
}

Vector3i::Vector3i(::std::initializer_list<coord_t> init) {
    if (init.size() != 3) {
        throw ::std::invalid_argument("Vector3i: Invalid size of initializer_list (requires exactly 3 elements).");
    }

    m_data = _mm_set_epi32(0, *(init.begin() + 2), *(init.begin() + 1), *init.begin());
}

Vector3i::Vector3i(__m128i data) : m_data(data) {
}

bool Vector3i::operator==(const Vector3i& o) const {
    __m128i xor_result = _mm_xor_si128(m_data, o.m_data);
    return _mm_test_all_zeros(xor_result, xor_result);
}

bool Vector3i::operator!=(const Vector3i& o) const {
    return !(*this == o);
}

coord_t Vector3i::operator[](uint i) const {
    // GCC does not allow variable i as a parameter for _mm_extract_epi32... hence we use that switch construct
    switch (i) {
        case 0:
            return _mm_extract_epi32(m_data, 0);
        case 1:
            return _mm_extract_epi32(m_data, 1);
        case 2:
            return _mm_extract_epi32(m_data, 2);
        default:
            throw ::std::out_of_range("Vector3i: Array access parameter out of bounds. Expected number between 0 and 2");
    }
}

Vector3i Vector3i::operator-() const {
    return Vector3i(_mm_sub_epi32(_mm_set1_epi32(0), m_data));
}

Vector3i& Vector3i::operator+=(const Vector3i& o) {
    m_data = _mm_add_epi32(m_data, o.m_data);
    return *this;
}

Vector3i& Vector3i::operator-=(const Vector3i& o) {
    m_data = _mm_sub_epi32(m_data, o.m_data);
    return *this;
}

Vector3i& Vector3i::operator*=(const coord_t& scalar) {
    m_data = _mm_mullo_epi32(m_data, _mm_set1_epi32(scalar));
    return *this;
}

Vector3i& Vector3i::max(const Vector3i& o) {
    m_data = _mm_max_epi32(m_data, o.m_data);
    return *this;
}

Vector3i& Vector3i::min(const Vector3i& o) {
    m_data = _mm_min_epi32(m_data, o.m_data);
    return *this;
}

bool Vector3i::operator<(const Vector3i& o) const {

    // Orders first by z, then y, then x
    int lessThanOther = _mm_movemask_epi8(_mm_cmplt_epi32(m_data, o.m_data));
    int greaterThanOther = _mm_movemask_epi8(_mm_cmpgt_epi32(m_data, o.m_data));

    return lessThanOther > greaterThanOther;
}

Vector3i Vector3i::abs() const {
    return Vector3i(_mm_abs_epi32(m_data));
}

coord_t Vector3i::dot(const Vector3i& o) const {
    __m128i tmp = _mm_mullo_epi32(m_data, o.m_data);
    // _mm_slli_si128(a, b) shifts a right by b bytes
    tmp = _mm_add_epi32(tmp, _mm_srli_si128(tmp, 8)); // (0, x, y, z) + (0, 0, 0, x) = (0, x, y, z + x)
    tmp = _mm_add_epi32(tmp, _mm_srli_si128(tmp, 4)); // (0, x, y, z + x) + (0, 0, x, y) = (0, x, y + x, z + x + y)
    return _mm_cvtsi128_si32(tmp); // returns the least significant 32 bits (the x component of tmp which holds the sum)
}

coord_t Vector3i::x() const {
    return _mm_extract_epi32(m_data, 0);
}

void Vector3i::setX(coord_t value) {
    m_data = _mm_insert_epi32(m_data, value, 0);
}

coord_t Vector3i::y() const {
    return _mm_extract_epi32(m_data, 1);
}

void Vector3i::setY(coord_t value) {
    m_data = _mm_insert_epi32(m_data, value, 1);
}

coord_t Vector3i::z() const {
    return _mm_extract_epi32(m_data, 2);
}

void Vector3i::setZ(coord_t value) {
    m_data = _mm_insert_epi32(m_data, value, 2);
}

#else
Vector3i::Vector3i() : m_x(0), m_y(0), m_z(0) {
}

Vector3i::Vector3i(coord_t x, coord_t y, coord_t z) : m_x(x), m_y(y), m_z(z) {
}

Vector3i::Vector3i(coord_t v) : m_x(v), m_y(v), m_z(v) {
}

Vector3i::Vector3i(::std::initializer_list<coord_t> init) : m_x(0), m_y(0), m_z(0) {
    if (init.size() != 3) {
        throw ::std::invalid_argument("Vector3i: Invalid size of initializer_list (requires exactly 3 elements).");
    }

    auto it = init.begin();

    m_x = *it;
    it++;
    m_y = *it;
    it++;
    m_z = *it;
}

bool Vector3i::operator==(const Vector3i& o) const {
    return m_x == o.m_x && m_y == o.m_y && m_z == o.m_z;
}

bool Vector3i::operator!=(const Vector3i& o) const {
    return !(*this == o);
}

coord_t Vector3i::operator[](uint i) const {
    switch (i) {
        case 0:
            return m_x;
        case 1:
            return m_y;
        case 2:
            return m_z;
        default:
            throw ::std::out_of_range("Vector3i: Array access parameter out of bounds. Expected number between 0 and 2");
    }
}

Vector3i Vector3i::operator-() const {
    return Vector3i(-m_x, -m_y, -m_z);
}

bool Vector3i::operator<(const Vector3i& o) const {
    if (o.m_x > this->m_x) {
        return true;
    } else if (o.m_x < this->m_x) {
        return false;
    }

    if (o.m_y > this->m_y) {
        return true;
    } else if (o.m_y < this->m_y) {
        return false;
    }

    return o.m_z > this->m_z;
}

Vector3i& Vector3i::operator+=(const Vector3i& o) {
    m_x += o.x();
    m_y += o.y();
    m_z += o.z();
    return *this;
}

Vector3i& Vector3i::operator-=(const Vector3i& o) {
    m_x -= o.x();
    m_y -= o.y();
    m_z -= o.z();
    return *this;
}

Vector3i& Vector3i::operator*=(const coord_t& o) {
    m_x *= o;
    m_y *= o;
    m_z *= o;
    return *this;
}

Vector3i& Vector3i::max(const Vector3i& o) {
    m_x = std::max(m_x, o.x());
    m_y = std::max(m_y, o.y());
    m_z = std::max(m_z, o.z());
    return *this;
}

Vector3i& Vector3i::min(const Vector3i& o) {
    m_x = std::min(m_x, o.x());
    m_y = std::min(m_y, o.y());
    m_z = std::min(m_z, o.z());
    return *this;
}

Vector3i Vector3i::abs() const {
    return Vector3i(::std::abs(m_x), ::std::abs(m_y), ::std::abs(m_z));
}

coord_t Vector3i::dot(const Vector3i& o) const {
    return m_x * o.m_x + m_y * o.m_y + m_z * o.m_z;
}

coord_t Vector3i::x() const {
    return m_x;
}

void Vector3i::setX(coord_t value) {
    m_x = value;
}

coord_t Vector3i::y() const {
    return m_y;
}

void Vector3i::setY(coord_t value) {
    m_y = value;
}

coord_t Vector3i::z() const {
    return m_z;
}

void Vector3i::setZ(coord_t value) {
    m_z = value;
}
#endif

Vector3i operator+(Vector3i a, const Vector3i& b) {
    return a += b;
}

Vector3i operator-(Vector3i a, const Vector3i& b) {
    return a -= b;
}

Vector3i operator*(Vector3i a, coord_t scalar) {
    return a *= scalar;
}

Vector3i operator*(coord_t scalar, Vector3i a) {
    return a *= scalar;
}

Vector3i max(Vector3i a, const Vector3i& b) {
    return a.max(b);
}

Vector3i min(Vector3i a, const Vector3i& b) {
    return a.min(b);
}

::std::ostream& operator<<(::std::ostream& s, const Vector3i& c) {
    s << "(" << c.x() << ", " << c.y() << ", " << c.z() << ")";
    return s;
}
}
