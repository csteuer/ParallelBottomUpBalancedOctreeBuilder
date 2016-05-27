#pragma once

#include "vector3i.h"

#include <stdint.h>

namespace octreebuilder {

typedef uint64_t morton_t;

// Guarantees that each morton encoded coordinate can be represented by a Vector3i (but not vice versa!).
static_assert(sizeof(morton_t) / 3 < sizeof(coord_t), "Data type coord_t must be big enough to represent all morton codes.");
}
