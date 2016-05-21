# - Find TCMALLOC
# Find the native TCMALLOC includes and library
#
#  TCMALLOC_LIBRARIES   - List of libraries when using TCMALLOC.
#  TCMALLOC_FOUND       - True if TCMALLOC found.

find_library(TCMALLOC_LIBRARIES NAMES tcmalloc_minimal)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(TCMALLOC REQUIRED_VARS TCMALLOC_LIBRARIES)
mark_as_advanced(TCMALLOC_LIBRARIES)
