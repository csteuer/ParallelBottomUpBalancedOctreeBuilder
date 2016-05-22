#include "vector_utils.h"

#include "box.h"

VectorSpace::VectorSpace(const Vector3i& start, const Vector3i& end) {
    init(start, end);
}

VectorSpace::VectorSpace(const Vector3i& end) {
    init(Vector3i(0), end);
}

VectorSpace::VectorSpace(const Box& box) {
    init(box.llf(), box.urb());
}

void VectorSpace::init(const Vector3i& start, const Vector3i& end) {
    m_start = start;
    m_end = end;

    if (max(start, end) != end) {
        m_end = m_start;
    }
}

VectorSpace::VoxelRangeIter VectorSpace::begin() const {
    return VoxelRangeIter(m_start, m_end);
}

VectorSpace::VoxelRangeIter VectorSpace::end() const {
    return VoxelRangeIter(m_end, m_end);
}

coord_t VectorSpace::size() const {
    return Box(m_start, m_end).volume();
}

bool VectorSpace::empty() const {
    return m_start == m_end;
}

VectorSpace::VoxelRangeIter::VoxelRangeIter(const Vector3i& start, const Vector3i& end) : m_current(start), m_start(start), m_end(end) {
}

bool VectorSpace::VoxelRangeIter::operator!=(const VectorSpace::VoxelRangeIter& other) const {
    return m_current != other.m_current;
}

bool VectorSpace::VoxelRangeIter::operator==(const VectorSpace::VoxelRangeIter& other) const {
    return m_current == other.m_current;
}

const VectorSpace::value_type& VectorSpace::VoxelRangeIter::operator*() const {
    return m_current;
}

const VectorSpace::VoxelRangeIter& VectorSpace::VoxelRangeIter::operator++() {
    m_current.setX(m_current.x() + 1);

    if (m_current.x() >= m_end.x()) {
        m_current.setX(m_start.x());
        m_current.setY(m_current.y() + 1);
    } else {
        return *this;
    }

    if (m_current.y() >= m_end.y()) {
        m_current.setY(m_start.y());
        m_current.setZ(m_current.z() + 1);
    } else {
        return *this;
    }

    if (m_current.z() >= m_end.z()) {
        // The end z is reached... since its exclusive set current to end so that this iterator is equal to the end-iterator
        m_current = m_end;
    }

    return *this;
}

VectorSpace ClosedVectorSpace(const Vector3i& start, const Vector3i& end) {
    return VectorSpace(start, end + Vector3i(1));
}

VectorSpace ClosedVectorSpace(const Box& box) {
    return ClosedVectorSpace(box.llf(), box.urb());
}

VectorSpace ClosedVectorSpace(const Vector3i& end) {
    return VectorSpace(end + Vector3i(1));
}
