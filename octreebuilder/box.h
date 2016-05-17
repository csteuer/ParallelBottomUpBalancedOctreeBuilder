#pragma once

#include <array>
#include <iosfwd>

#include "octreebuilder_api.h"
#include "vector3i.h"

class OCTREEBUILDER_API Box {
public:
    Box();
    Box(const Vector3i& llf, const Vector3i& urb);
    explicit Box(const Vector3i& urb);

    const Vector3i& llf() const;
    const Vector3i& urb() const;
    Vector3i diagonal() const;

    /**
     * @brief returns the number of voxels inside the box (voxel == unit cube)
     * @return the number of voxels inside the box where the voxel at urb is not part of the box (e.g. Box({ 0, 0, 0 }, {0, 0, 0}).volume() == 0)
     */
    coord_t volume() const;

    /**
     * @brief return true if box contains at least one voxel
     */
    bool valid() const;

    bool operator==(const Box& other) const;

    bool operator!=(const Box& other) const;

    bool contains(const Box& other) const;

    /**
     * @brief tests wether the bounding box contains the point
     */
    bool contains(const Vector3i& point) const;

private:
    Vector3i m_llf;
    Vector3i m_urb;
};

OCTREEBUILDER_API std::ostream& operator<<(std::ostream& s, const Box& b);
