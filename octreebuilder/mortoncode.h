#pragma once

#include "vector3i.h"

#include <stdint.h>

namespace octreebuilder {

typedef uint64_t morton_t;

// Guarantees that each morton encoded coordinate can be represented by a Vector3i (but not vice versa!).
static_assert(sizeof(morton_t) == sizeof(coord_t), "Data type coord_t must have same size as coord_t.");
}
