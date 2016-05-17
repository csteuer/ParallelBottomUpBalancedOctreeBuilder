#include "vector3i.h"

#include <stdexcept>
#include <cmath>
#include <ostream>

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
            throw std::runtime_error("VoxelCoordinate: Array access parameter out of bounds. Expected number between 0 and 2");
    }
}

coord_t& Vector3i::operator[](uint i) {
    switch (i) {
        case 0:
            return m_x;
        case 1:
            return m_y;
        case 2:
            return m_z;
        default:
            throw std::runtime_error("VoxelCoordinate: Array access parameter out of bounds. Expected number between 0 and 2");
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

Vector3i Vector3i::abs() const {
    return Vector3i(std::abs(m_x), std::abs(m_y), std::abs(m_z));
}

coord_t Vector3i::dot(const Vector3i& other) const {
    return m_x * other.m_x + m_y * other.m_y + m_z * other.m_z;
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

std::ostream& operator<<(std::ostream& s, const Vector3i& c) {
    s << "(" << c.x() << ", " << c.y() << ", " << c.z() << ")";
    return s;
}

Vector3i::Vector3i(std::initializer_list<coord_t> init) : m_x(0), m_y(0), m_z(0) {
    if (init.size() != 3) {
        throw std::runtime_error("VoxelCoordinate: Invalid size of initializer_list (requires exactly 3 elements).");
    }

    auto it = init.begin();

    m_x = *it;
    it++;
    m_y = *it;
    it++;
    m_z = *it;
}

Vector3i operator+(const Vector3i& a, const Vector3i& b) {
    return Vector3i(a.x() + b.x(), a.y() + b.y(), a.z() + b.z());
}

Vector3i operator-(const Vector3i& a, const Vector3i& b) {
    return Vector3i(a.x() - b.x(), a.y() - b.y(), a.z() - b.z());
}

Vector3i operator*(const Vector3i& c, const coord_t& scalar) {
    return Vector3i(c.x() * scalar, c.y() * scalar, c.z() * scalar);
}

Vector3i operator*(const coord_t& scalar, const Vector3i& c) {
    return c * scalar;
}

Vector3i max(const Vector3i& a, const Vector3i& b) {
    return Vector3i(std::max(a.x(), b.x()), std::max(a.y(), b.y()), std::max(a.z(), b.z()));
}

Vector3i min(const Vector3i& a, const Vector3i& b) {
    return Vector3i(std::min(a.x(), b.x()), std::min(a.y(), b.y()), std::min(a.z(), b.z()));
}
