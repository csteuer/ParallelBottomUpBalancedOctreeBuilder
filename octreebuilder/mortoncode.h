#pragma once

/**
  * Implements morton encoding/decoding (also known as z-order curve).
  * Can be used to address the octants of an octree.
  */

#include "octreebuilder_api.h"

#include "vector3i.h"

#include <array>
#include <vector>

typedef uint64_t morton_t;

// Guarantees that each morton encoded coordinate can be represented by a Vector3i (but not vice versa!).
static_assert(sizeof(morton_t) == sizeof(coord_t), "Data type coord_t must have same size as coord_t.");

/**
 * @brief tests wether all coordinates in the bounding box can be morton encoded
 * @param maxXYZ the urb of the bounding box with llf (0,0,0)
 * @return true if all coordinates can be encoded and false otherwise
 */
OCTREEBUILDER_API bool fitsInMortonCode(const Vector3i& maxXYZ);

/**
 * @brief computes the depth of an octree that partitions the bounding box (leave octants are cubes with volume 1)
 * @param maxXYZ the urb of the bounding box with llf (0,0,0)
 * @return the depth of the octree
 */
OCTREEBUILDER_API uint getOctreeDepthForBounding(const Vector3i& maxXYZ);

/**
 * @brief urb of the bounding box of an octree of a given depth
 * @param depth the depth of the octree
 */
OCTREEBUILDER_API Vector3i getMaxXYZForOctreeDepth(const uint& depth);

/**
 * @brief the size of octants at level
 * @param level the level inside the octree
 * @return the side length of the octants at that level
 */
OCTREEBUILDER_API coord_t getOctantSizeForLevel(const uint& level);

/**
 * @brief computes the morton code for a coordinate
 */
OCTREEBUILDER_API morton_t getMortonCodeForCoordinate(const Vector3i& coordinate);

/**
 * @brief computes the coordinate represented by the morton code
 */
OCTREEBUILDER_API Vector3i getCoordinateForMortonCode(const morton_t& code);

/**
 * @brief computes the octant from the next higher level that contains the octant of the current level (goes up)
 * @param current_code the morton encoded llf of the octant on the current level (or any morton encoded coordinate within this octant)
 * @param currentLevel the current level in the octree
 */
OCTREEBUILDER_API morton_t getMortonCodeForParent(const morton_t& current_code, const uint& currentLevel);

/**
 * @brief computes the octant from the ancestor level that contains the octant of the current level (goes up)
 * @param current_code the morton encoded llf of the octant on the current level (or any morton encoded coordinate within this octant)
 * @param currentLevel the level of the octant in the octree
 * @param ancestorLevel the level of the ancestor in the octree (must be greater than currentLevel otherwise an exception is thrown)
 *
 * Another way to get the ancestor is calling getMortonCodeForParentLevel recursively. However this method is more efficient.
 */
OCTREEBUILDER_API morton_t getMortonCodeForAncestor(const morton_t& current_code, const uint& currentLevel, const uint& ancestorLevel);

/**
 * @brief computes the morton encoded children of the octant
 * @param parent the morton encoded llf of the parent octant on the parentLevel (or any morton encoded coordinate within this octant)
 * @param parentLevel the level of the parent octant in the octree
 * @return the 8 children of the parent octant ordered as follows (the coordinates beeing relative to the LLF of the parent octant and its size):
 *         [(0,0,0),(0,0,1),(0,1,0),(0,1,1),(1,0,0),(1,0,1),(1,1,0),(1,1,1)]
 */
OCTREEBUILDER_API std::array<morton_t, 8> getMortonCodesForChildren(const morton_t& parent, const uint& parentLevel);

/**
 * @brief computes the morton encoded 26-connected neighbours of the current octant (Moore neighborhood)
 * @param current_octant the octant for which the neighbours should be calculated
 * @param currentLevel the level of the current octant (and its neighbours)
 * @param octreeDepth the depth of the octree
 * @return all octants that are inside the bounds of the tree (given by its depth) and touch a face, a edge or a vertex of the current octant
 */
OCTREEBUILDER_API std::vector<morton_t> getMortonCodesForNeighbourOctants(const morton_t& current_octant, const uint& currentLevel, const uint& octreeDepth,
                                                                          const Vector3i& root = Vector3i(0));

/**
 * @brief the maximum level of all possible octants with the given llf
 * @param llf the llf
 * @param octreeDepth the depth of the octree
 */
OCTREEBUILDER_API uint getMaxLevelOfLLF(const Vector3i& llf, const uint& octreeDepth);

/**
 * @brief returns the corners of the octant which could be adjacent to unbalanced neighbour octant
 * @param octant the morton code of the octant
 * @param level the level of the octant
 * @return the coordinates of the search corners
 *
 * The search corners of an octant are the cornes it shares with none of its siblings.
 */
OCTREEBUILDER_API Vector3i getSearchCorner(const morton_t& octant, const uint& level);

/**
 * @brief tests whether octant is the decendant of potentialAncestor
 * @param octant the morton code of the decendant
 * @param levelOfOctant the level of the decendant
 * @param potentialAncestor the morton code of the potential ancestor
 * @param levelOfAncestor the level of the potential ancestor
 * @return true of octant is a decendant of potentialAncestor, false otherwise
 */
OCTREEBUILDER_API bool isMortonCodeDecendant(const morton_t& octant, const uint& levelOfOctant, const morton_t& potentialAncestor, const uint& levelOfAncestor);

/**
 * @brief returns the nearest common ancestor of octant a and octant b
 * @param a morton code of octant a
 * @param b morton code of octant b
 * @param aLevel level of octant a
 * @param bLevel level of octant b
 * @return the morton code (first) and level (second) of the nearest common ancestor of a and b
 */
OCTREEBUILDER_API std::pair<morton_t, uint> nearestCommonAncestor(const morton_t& a, const morton_t& b, const uint& aLevel, const uint& bLevel);
