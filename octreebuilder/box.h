#pragma once

#include <limits>
#include <iosfwd>

#include "octreebuilder_api.h"
#include "vector3i.h"

namespace octreebuilder {

class OCTREEBUILDER_API Box {
public:
    constexpr Box() : m_llf(Vector3i(::std::numeric_limits<coord_t>::max())), m_urb(::std::numeric_limits<coord_t>::min()) {
    }
    constexpr Box(const Vector3i& llf, const Vector3i& urb) : m_llf(llf), m_urb(urb) {
    }
    explicit constexpr Box(const Vector3i& urb) : m_llf(Vector3i(0)), m_urb(urb) {
    }

    const Vector3i& llf() const;
    const Vector3i& urb() const;
    Vector3i diagonal() const;

    coord_t volume() const;

    /**
     * @brief Returns true if the volume is greater than zero
     */
    bool valid() const;

    bool operator==(const Box& other) const;

    bool operator!=(const Box& other) const;

    bool contains(const Box& other) const;

    bool contains(const Vector3i& point) const;

private:
    Vector3i m_llf;
    Vector3i m_urb;
};

OCTREEBUILDER_API ::std::ostream& operator<<(::std::ostream& s, const Box& b);
}
