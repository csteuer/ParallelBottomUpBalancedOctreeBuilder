#pragma once

#include "octreebuilder_api.h"

#include "vector3i.h"

#include <iterator>

class Box;

class OCTREEBUILDER_API VectorSpace {
public:

    class VoxelRangeIter;
    typedef Vector3i value_type;
    typedef VoxelRangeIter const_iterator;
    typedef coord_t size_type;

    /**
     * @brief a voxel range from start to end (end is exclusive => [start, end) )
     * @param start the start of the range
     * @param end the exclusive end of the range
     */
    VectorSpace(const Vector3i& start, const Vector3i& end);

    /**
     * @brief a voxel range from (0,0,0) to end
     * @param end the exclusive end of the range
     */
    explicit VectorSpace(const Vector3i& end);

    /**
     * @brief a voxel range from box.llf to box.urb [box.llf, box.urb)
     * @param box the box
     */
    explicit VectorSpace(const Box& box);

    class OCTREEBUILDER_API VoxelRangeIter : public std::iterator<std::input_iterator_tag, Vector3i> {
    public:
        VoxelRangeIter(const Vector3i& start, const Vector3i& end);
        bool operator!= (const VoxelRangeIter& other) const;
        bool operator== (const VoxelRangeIter& other) const;
        const value_type& operator* () const;
        const VoxelRangeIter& operator++();
    private:
        Vector3i m_current;
        const Vector3i m_start;
        const Vector3i m_end;
    };

    VoxelRangeIter begin() const;
    VoxelRangeIter end() const;

    coord_t size() const;
    bool empty() const;

private:
    void init(const Vector3i& start, const Vector3i& end);
    Vector3i m_start;
    Vector3i m_end;
};

/**
 * @brief creates a voxel range from start to end (end is inclusive => [start, end] )
 * @param start the start of the range
 * @param end the end of the voxel range
 */
OCTREEBUILDER_API VectorSpace ClosedVectorSpace(const Vector3i& start, const Vector3i& end);

/**
 * @brief creates a voxel range from (0,0,0) to end (end is inclusive => [(0,0,0), end] )
 * @param end the end of the voxel range
 */
OCTREEBUILDER_API VectorSpace ClosedVectorSpace(const Vector3i& end);
