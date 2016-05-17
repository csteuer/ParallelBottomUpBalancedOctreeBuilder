#include "box.h"
#include <stdexcept>
#include <unordered_map>
#include <limits>
#include <ostream>

Box::Box() : m_llf(Vector3i(std::numeric_limits<coord_t>::max())), m_urb(std::numeric_limits<coord_t>::min()) {
}

Box::Box(const Vector3i& llf, const Vector3i& urb) : m_llf(llf), m_urb(urb) {
}

Box::Box(const Vector3i& urb) : m_llf(Vector3i(0)), m_urb(urb) {
}

const Vector3i& Box::llf() const {
    return m_llf;
}

const Vector3i& Box::urb() const {
    return m_urb;
}

Vector3i Box::diagonal() const {
    return m_urb - m_llf;
}

coord_t Box::volume() const {
    if (!valid()) {
        return 0;
    }

    Vector3i diagonal = this->diagonal();

    return diagonal.x() * diagonal.y() * diagonal.z();
}

bool Box::valid() const {
    return m_urb.x() > m_llf.x() && m_urb.y() > m_llf.y() && m_urb.z() > m_llf.z();
}

bool Box::operator==(const Box& other) const {
    return m_llf == other.llf() && m_urb == other.urb();
}

bool Box::operator!=(const Box& other) const {
    return !(*this == other);
}

bool Box::contains(const Box& other) const {
    return min(m_llf, other.llf()) == m_llf && max(m_urb, other.urb()) == m_urb;
}

bool Box::contains(const Vector3i& point) const {
    return point.x() >= m_llf.x() && point.y() >= m_llf.y() && point.z() >= m_llf.z() && m_urb.x() >= point.x() && m_urb.y() >= point.y() &&
           m_urb.z() >= point.z();
}

std::ostream& operator<<(std::ostream& s, const Box& b) {
    s << "{ llf: " << b.llf() << ", urb: " << b.urb() << " }";
    return s;
}
