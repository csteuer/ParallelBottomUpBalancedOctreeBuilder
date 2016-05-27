#pragma once

#include <limits>
#include <iosfwd>

#include "octreebuilder_api.h"
#include "vector3i.h"

namespace octreebuilder {

class OCTREEBUILDER_API Box {
public:
    Box();
    Box(const Vector3i& llf, const Vector3i& urb);
    explicit Box(const Vector3i& urb);

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

OCTREEBUILDER_API::std::ostream& operator<<(::std::ostream& s, const Box& b);
}
