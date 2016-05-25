#include "mortoncode_utils.h"

#include "box.h"

#include <stdexcept>
#include <limits>
#include <assert.h>

namespace octreebuilder {

/**
 * @brief returns the number of the most significant bit that is set
 * @param number the number in which the position of the most significant set bit is searched
 * @return the position of the most significant set bit where the least significant bit position has index 0
 *
 * e.g.:
 *  getMostSignigicantSetBitPos(4) <=> getMostSignigicantSetBitPos(...100) => 2
 *  getMostSignigicantSetBitPos(5) <=> getMostSignigicantSetBitPos(...101) => 2
 *  getMostSignigicantSetBitPos(8) <=> getMostSignigicantSetBitPos(...1000) => 3
 */
static int getMostSignigicantSetBitPos(morton_t number) {
    if (number == 0) {
        return -1;
    }

#ifdef MSVC
    unsigned long result;
    _BitScanReverse64(&result, number);
    return 63 - static_cast<int>(result);
#else
    return 63 - __builtin_clzll(static_cast<unsigned long long>(number));
#endif
}

static int getMostSignigicantSetBitPos(coord_t number) {
    if (number < 0) {
        throw ::std::runtime_error("Invalid input.");
    }

    return getMostSignigicantSetBitPos(static_cast<morton_t>(number));
}

/**
 * @brief returns the number of the least significant bit that is set
 * @param number the number in which the position of the least significant set bit is searched
 * @return the position of the least significant set bit where the least significant bit position has index 0
 *
 * e.g.:
 *  getLeastSignigicantSetBitPos(4) <=> getLeastSignigicantSetBitPos(...100) => 2
 *  getLeastSignigicantSetBitPos(5) <=> getLeastSignigicantSetBitPos(...101) => 0
 *  getLeastSignigicantSetBitPos(8) <=> getLeastSignigicantSetBitPos(...1000) => 3
 *  getLeastSignigicantSetBitPos(10) <=> getLeastSignigicantSetBitPos(...1010) => 1
 */
static int getLeastSignigicantSetBitPos(coord_t number) {
    if (number < 0) {
        throw ::std::runtime_error("Invalid input.");
    }

    if (number == 0) {
        return -1;
    }

#ifdef MSVC
    unsigned long result;
    _BitScanForward64(&result, number);
    return static_cast<int>(result);
#else
    return __builtin_ctzll(static_cast<unsigned long long>(number));
#endif
}

constexpr size_t maxBitsPerComponent = 21;  // (sizeof(morton_t) * 8) / 3;

bool fitsInMortonCode(const Vector3i& maxXYZ) {
    coord_t max = ::std::max(maxXYZ.x(), ::std::max(maxXYZ.y(), maxXYZ.z()));

    // when shifting all allowed bits out the result must be 0
    return (max >> maxBitsPerComponent) == 0;
}

uint getOctreeDepthForBounding(const Vector3i& maxXYZ) {
    coord_t max = ::std::max(maxXYZ.x(), ::std::max(maxXYZ.y(), maxXYZ.z()));

    if (max < 0) {
        throw ::std::runtime_error("Invalid bounding (all components must not be negative)");
    }

    // how many bits are required to store numbers from 0 to max
    int requiredBits = getMostSignigicantSetBitPos(max) + 1;

    return static_cast<uint>(requiredBits);
}

Vector3i getMaxXYZForOctreeDepth(const uint& depth) {
    coord_t size = (1 << depth) - 1;
    return Vector3i(size);
}

coord_t getOctantSizeForLevel(const uint& level) {
    return 1 << level;
}

#define S4(i) S1((i)), S1((i + 1)), S1((i + 2)), S1((i + 3))
#define S16(i) S4(i), S4(i + 4), S4(i + 8), S4(i + 12)
#define S64(i) S16(i), S16(i + 16), S16(i + 32), S16(i + 48)
#define S256 S64(0), S64(64), S64(128), S64(192)

constexpr uint32_t x_component_lut[256] = {
#define S1(x) (x & 1) << 2 | (x & 2) << 4 | (x & 4) << 6 | (x & 8) << 8 | (x & 16) << 10 | (x & 32) << 12 | (x & 64) << 14 | (x & 128) << 16
    S256
#undef S1
};

constexpr uint32_t y_component_lut[256] = {
#define S1(x) (x & 1) << 1 | (x & 2) << 3 | (x & 4) << 5 | (x & 8) << 7 | (x & 16) << 9 | (x & 32) << 11 | (x & 64) << 13 | (x & 128) << 15
    S256
#undef S1
};

constexpr uint32_t z_component_lut[256] = {
#define S1(x) (x & 1) | (x & 2) << 2 | (x & 4) << 4 | (x & 8) << 6 | (x & 16) << 8 | (x & 32) << 10 | (x & 64) << 12 | (x & 128) << 14
    S256
#undef S1
};

morton_t getMortonCodeForCoordinate(const Vector3i& coordinate) {
    morton_t mcode =
        x_component_lut[(coordinate.x() >> 16) & 0xFF] | y_component_lut[(coordinate.y() >> 16) & 0xFF] | z_component_lut[(coordinate.z() >> 16) & 0xFF];
    mcode = mcode << 48 | x_component_lut[(coordinate.x() >> 8) & 0xFF] | y_component_lut[(coordinate.y() >> 8) & 0xFF] |
            z_component_lut[(coordinate.z() >> 8) & 0xFF];
    mcode = mcode << 24 | x_component_lut[(coordinate.x()) & 0xFF] | y_component_lut[(coordinate.y()) & 0xFF] | z_component_lut[(coordinate.z()) & 0xFF];
    return mcode;
}

/**
 * @brief uses only every third bit
 *
 * E.g.:
 *  1001001 => 111
 *  1000001 => 101
 */
static morton_t removeSpaceBetweenBits(morton_t spaced) {
    morton_t result = 0;

    // just do the reverse of makeSpaceBetweenBits
    for (size_t i = 0; i < maxBitsPerComponent; i++) {
        result |= (spaced >> (i * 2)) & (1 << i);
    }

    assert(result <= ::std::numeric_limits<coord_t>::max());

    return result;
}

Vector3i getCoordinateForMortonCode(const morton_t& code) {
    morton_t z = removeSpaceBetweenBits(code);
    morton_t y = removeSpaceBetweenBits(code >> 1);
    morton_t x = removeSpaceBetweenBits(code >> 2);

    return Vector3i(static_cast<coord_t>(x), static_cast<coord_t>(y), static_cast<coord_t>(z));
}

morton_t getMortonCodeForParent(const morton_t& current_code, const uint& currentLevel) {
    // sets all bits to one and then shift 3 times current level + 1 to the right. This yields a mask that removes all bits from the current level and below
    morton_t parentLevelBitMask = morton_t(-1) << 3 * (currentLevel + 1);
    morton_t parentLevelCode = current_code & parentLevelBitMask;
    return parentLevelCode;
}

morton_t getMortonCodeForAncestor(const morton_t& current_code, const uint& currentLevel, const uint& ancestorLevel) {
    if (ancestorLevel < currentLevel) {
        throw ::std::runtime_error("getMortonCodeForAncestor: Invalid parameter. Current level is greater than ancesor level.");
    }

    if (ancestorLevel == currentLevel) {
        return current_code;
    }

    morton_t parentLevelBitMask = morton_t(-1) << 3 * ancestorLevel;
    morton_t parentLevelCode = current_code & parentLevelBitMask;
    return parentLevelCode;
}

::std::array<morton_t, 8> getMortonCodesForChildren(const morton_t& parent, const uint& parentLevel) {
    if (parentLevel == 0) {
        throw ::std::runtime_error("Leaf octants can't have children.");
    }

    uint childLevel = parentLevel - 1;

    // 'parent' parameter could be any morton encoded coordinate in side the parent actual parent
    // which is determined by the bit triplets at position 'parentLevel' and before. Do as if 'parent' is indeed the
    // llf of a child and use getMortonCodeForParentLevel to obtain the "normalized" parent_llf
    morton_t parent_llf = getMortonCodeForParent(parent, childLevel);

    ::std::array<morton_t, 8> children;

    // The children llf's are defined by the bit-triplet at position 'childLevel' e.g. '<parent-bits> 000 ... 000'
    // if the lower left front child while '<parent-bits> 001 ... 000' is the upper left front child and so on
    // Hence bit generate the bit-sequences from 0 to 7 (000, 001, 010, 011...) and put them at the 'childLevel' position
    for (morton_t childIndex = 0; childIndex < 8; childIndex++) {
        morton_t childBitTriplet = childIndex << (childLevel * 3);

        morton_t child = parent_llf | childBitTriplet;

        children.at(childIndex) = child;
    }
    return children;
}

::std::vector<morton_t> getMortonCodesForNeighbourOctants(const morton_t& current_octant, const uint& currentLevel, const uint& octreeDepth,
                                                        const Vector3i& root) {
    if (currentLevel > octreeDepth) {
        throw ::std::runtime_error("No level must be greater than the octreeDepth.");
    }

    Box octreeBounds = Box(root, root + getMaxXYZForOctreeDepth(octreeDepth));

    // make sure that we get the llf of the octant at the current level
    morton_t currentLevelMask = morton_t(-1) << 3 * currentLevel;
    morton_t currentOctantMaskedCode = current_octant & currentLevelMask;

    Vector3i currentOctant_llf = getCoordinateForMortonCode(currentOctantMaskedCode);

    if (!octreeBounds.contains(currentOctant_llf)) {
        throw ::std::runtime_error("Octant not in octree.");
    }

    coord_t octantSize = getOctantSizeForLevel(currentLevel);

    ::std::vector<morton_t> neighbours;
    neighbours.reserve(26);

    const ::std::array<coord_t, 3> offsetRange = {{-octantSize, 0, octantSize}};

    for (coord_t offset_x : offsetRange) {
        for (coord_t offset_y : offsetRange) {
            for (coord_t offset_z : offsetRange) {
                Vector3i offset(offset_x, offset_y, offset_z);

                if (offset == Vector3i(0, 0, 0)) {
                    continue;
                }

                Vector3i neighbour_llf = currentOctant_llf + offset;

                if (!octreeBounds.contains(neighbour_llf)) {
                    continue;
                }

                neighbours.push_back(getMortonCodeForCoordinate(neighbour_llf));

                assert(octreeBounds.contains(getCoordinateForMortonCode(neighbours.back())));
            }
        }
    }

    assert(neighbours.size() <= 26);

    return neighbours;
}

uint getMaxLevelOfLLF(const Vector3i& llf, const uint& octreeDepth) {
    if (llf == Vector3i(0)) {
        return octreeDepth;
    }

    uint level = ::std::min(static_cast<uint>(getLeastSignigicantSetBitPos(llf.x())),
                          ::std::min(static_cast<uint>(getLeastSignigicantSetBitPos(llf.y())), static_cast<uint>(getLeastSignigicantSetBitPos(llf.z()))));

    if (level > octreeDepth) {
        throw ::std::runtime_error("Given llf out of bounds for an octree with the specified depth.");
    }

    return level;
}

Vector3i getSearchCorner(const morton_t& octant, const uint& level) {
    Vector3i llf = getCoordinateForMortonCode(octant);
    coord_t size = getOctantSizeForLevel(level);

    morton_t relative_location = (octant >> level * 3) & 7;

    switch (relative_location) {
        case 1:
            llf.setZ(llf.z() + size);
            break;
        case 2:
            llf.setY(llf.y() + size);
            break;
        case 3:
            llf.setY(llf.y() + size);
            llf.setZ(llf.z() + size);
            break;
        case 4:
            llf.setX(llf.x() + size);
            break;
        case 5:
            llf.setX(llf.x() + size);
            llf.setZ(llf.z() + size);
            break;
        case 6:
            llf.setX(llf.x() + size);
            llf.setY(llf.y() + size);
            break;
        case 7:
            llf.setX(llf.x() + size);
            llf.setY(llf.y() + size);
            llf.setZ(llf.z() + size);
            break;
        default:
            break;
    }

    return llf;
}

bool isMortonCodeDecendant(const morton_t& octant, const uint& levelOfOctant, const morton_t& potentialAncestor, const uint& levelOfAncestor) {
    if (levelOfOctant >= levelOfAncestor) {
        return false;
    }

    if (potentialAncestor > octant) {
        return false;
    }

    morton_t potentialAncestorLevelBitMask = morton_t(-1) << 3 * levelOfAncestor;

    // Check that potentialAncestor morton code is valid
    assert(potentialAncestor == (potentialAncestor & potentialAncestorLevelBitMask));

    return (octant & potentialAncestorLevelBitMask) == potentialAncestor;
}

constexpr uint bitPosToLevelLookupTable[64] = {
#define S1(x) x / 3
    S64(0)
#undef S1
};

::std::pair<morton_t, uint> nearestCommonAncestor(const morton_t& a, const morton_t& b, const uint& aLevel, const uint& bLevel) {
    morton_t diff = a ^ b;  // a xor b => bit's are 1 where a and b are different
    int firstDiffPos = getMostSignigicantSetBitPos(diff);

    if (firstDiffPos < 0) {
        // a == b
        return ::std::make_pair(a, ::std::max(aLevel, bLevel));
    }

    uint nearestCommonAncestorLevel = ::std::max(bitPosToLevelLookupTable[firstDiffPos] + 1, ::std::max(aLevel, bLevel));

    morton_t nearestCommonAncestorBitMask = morton_t(-1) << 3 * nearestCommonAncestorLevel;
    morton_t nearestCommonAncestorCode = a & nearestCommonAncestorBitMask;
    return ::std::make_pair(nearestCommonAncestorCode, nearestCommonAncestorLevel);
}
}
