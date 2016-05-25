#pragma once

#include "octreebuilder_api.h"

#include "vector3i.h"

#include <iterator>

namespace octreebuilder {

class Box;

/**
 * @brief A class for creating squences of vectors
 */
class OCTREEBUILDER_API VectorSpace {
public:
    class VectorRangeIter;
    typedef Vector3i value_type;
    typedef VectorRangeIter const_iterator;
    typedef coord_t size_type;

    /**
     * @brief A vector range from start to end (end is exclusive => [start, end) )
     * @param start The start of the range
     * @param end The exclusive end of the range
     */
    VectorSpace(const Vector3i& start, const Vector3i& end);

    /**
     * @brief A vector range from (0,0,0) to end
     * @param end The exclusive end of the range
     */
    explicit VectorSpace(const Vector3i& end);

    /**
     * @brief A vector range from box.llf to box.urb [box.llf, box.urb)
     */
    explicit VectorSpace(const Box& box);

    class OCTREEBUILDER_API VectorRangeIter : public ::std::iterator<::std::input_iterator_tag, Vector3i> {
    public:
        VectorRangeIter(const Vector3i& start, const Vector3i& end);
        bool operator!=(const VectorRangeIter& other) const;
        bool operator==(const VectorRangeIter& other) const;
        const value_type& operator*() const;
        const VectorRangeIter& operator++();

    private:
        Vector3i m_current;
        const Vector3i m_start;
        const Vector3i m_end;
    };

    VectorRangeIter begin() const;
    VectorRangeIter end() const;

    coord_t size() const;
    bool empty() const;

private:
    void init(const Vector3i& start, const Vector3i& end);
    Vector3i m_start;
    Vector3i m_end;
};

/**
 * @brief Creates a vector range from start to end (end is inclusive => [start, end] )
 * @param start The start of the range
 * @param end The end of the vector range
 */
OCTREEBUILDER_API VectorSpace ClosedVectorSpace(const Vector3i& start, const Vector3i& end);

/**
 * @brief Creates a vector range from box (end is inclusive => [llf, urb] )
 */
OCTREEBUILDER_API VectorSpace ClosedVectorSpace(const Box& box);

/**
 * @brief Creates a vector range from (0,0,0) to end (end is inclusive => [(0,0,0), end] )
 * @param end The end of the vector range
 */
OCTREEBUILDER_API VectorSpace ClosedVectorSpace(const Vector3i& end);
}
