#pragma once

/**
  * Implements morton encoding/decoding (also known as z-order curve).
  * Morton codes can be used to address the octants of an octree.
  */

#include "octreebuilder_api.h"

#include "mortoncode.h"

#include <array>
#include <vector>

namespace octreebuilder {

/**
 * @brief Tests wether all coordinates in the bounding box can be morton encoded
 * @param maxXYZ The urb of the bounding box with llf (0,0,0)
 * @return true If all coordinates can be encoded and false otherwise
 */
OCTREEBUILDER_API bool fitsInMortonCode(const Vector3i& maxXYZ);

/**
 * @brief Computes the depth of an octree that partitions the bounding box (leave octants are cubes with volume 1)
 * @param maxXYZ The urb of the bounding box with llf (0,0,0)
 * @return The depth of the octree
 */
OCTREEBUILDER_API uint getOctreeDepthForBounding(const Vector3i& maxXYZ);

/**
 * @brief The upper right back coordinate of the bounding box of an octree of a given depth
 * @param depth The depth of the octree
 */
OCTREEBUILDER_API Vector3i getMaxXYZForOctreeDepth(const uint& depth);

/**
 * @brief The size of octants at level
 * @param level The level inside the octree
 * @return The side length of the octants at that level
 */
OCTREEBUILDER_API coord_t getOctantSizeForLevel(const uint& level);

/**
 * @brief Computes the morton code for a coordinate
 */
OCTREEBUILDER_API morton_t getMortonCodeForCoordinate(const Vector3i& coordinate);

/**
 * @brief Computes the coordinate represented by the morton code
 */
OCTREEBUILDER_API Vector3i getCoordinateForMortonCode(const morton_t& code);

/**
 * @brief Computes the octant from the next higher level that contains the octant of the current level (goes up)
 * @param current_code The morton encoded llf of the octant on the current level (or any morton encoded coordinate within this octant)
 * @param currentLevel The current level in the octree
 */
OCTREEBUILDER_API morton_t getMortonCodeForParent(const morton_t& current_code, const uint& currentLevel);

/**
 * @brief computes The octant from the ancestor level that contains the octant of the current level (goes up)
 * @param current_code The morton encoded llf of the octant on the current level (or any morton encoded coordinate within this octant)
 * @param currentLevel The level of the octant in the octree
 * @param ancestorLevel The level of the ancestor in the octree (must be greater than currentLevel otherwise an exception is thrown)
 *
 * Another way to get the ancestor is calling getMortonCodeForParentLevel recursively. However this method is more efficient.
 */
OCTREEBUILDER_API morton_t getMortonCodeForAncestor(const morton_t& current_code, const uint& currentLevel, const uint& ancestorLevel);

/**
 * @brief Computes the morton encoded children of the octant
 * @param parent The morton encoded llf of the parent octant on the parentLevel (or any morton encoded coordinate within this octant)
 * @param parentLevel The level of the parent octant in the octree
 * @return the 8 children of the parent octant ordered as follows (the coordinates beeing relative to the LLF of the parent octant and its size):
 *         [(0,0,0),(0,0,1),(0,1,0),(0,1,1),(1,0,0),(1,0,1),(1,1,0),(1,1,1)]
 */
OCTREEBUILDER_API ::std::array<morton_t, 8> getMortonCodesForChildren(const morton_t& parent, const uint& parentLevel);

/**
 * @brief Computes the morton encoded 26-connected neighbours of the current octant (Moore neighborhood)
 * @param current_octant The octant for which the neighbours should be calculated
 * @param currentLevel The level of the current octant (and its neighbours)
 * @param octreeDepth The depth of the octree
 * @return All octants that are inside the bounds of the tree (given by its depth) and touch a face, a edge or a vertex of the current octant
 */
OCTREEBUILDER_API ::std::vector<morton_t> getMortonCodesForNeighbourOctants(const morton_t& current_octant, const uint& currentLevel, const uint& octreeDepth,
                                                                          const Vector3i& root = Vector3i(0));

/**
 * @brief The maximum level of all possible octants with the given llf
 * @param llf The llf
 * @param octreeDepth The depth of the octree
 */
OCTREEBUILDER_API uint getMaxLevelOfLLF(const Vector3i& llf, const uint& octreeDepth);

/**
 * @brief Returns the corners of the octant which could be adjacent to unbalanced neighbour octant
 * @param octant The morton code of the octant
 * @param level The level of the octant
 * @return The coordinates of the search corners
 *
 * The search corners of an octant are the cornes it shares with none of its siblings.
 */
OCTREEBUILDER_API Vector3i getSearchCorner(const morton_t& octant, const uint& level);

/**
 * @brief Tests whether octant is the decendant of potentialAncestor
 * @param octant The morton code of the decendant
 * @param levelOfOctant The level of the decendant
 * @param potentialAncestor The morton code of the potential ancestor
 * @param levelOfAncestor The level of the potential ancestor
 * @return True of octant is a decendant of potentialAncestor, false otherwise
 */
OCTREEBUILDER_API bool isMortonCodeDecendant(const morton_t& octant, const uint& levelOfOctant, const morton_t& potentialAncestor, const uint& levelOfAncestor);

/**
 * @brief Returns the nearest common ancestor of octant a and octant b
 * @param a Morton code of octant a
 * @param b Morton code of octant b
 * @param aLevel Level of octant a
 * @param bLevel Level of octant b
 * @return The morton code (first) and level (second) of the nearest common ancestor of a and b
 */
OCTREEBUILDER_API ::std::pair<morton_t, uint> nearestCommonAncestor(const morton_t& a, const morton_t& b, const uint& aLevel, const uint& bLevel);
}
