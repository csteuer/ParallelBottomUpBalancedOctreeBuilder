set(target octreebuilder)

##########################################################################################################
# Lib Setup
##########################################################################################################

set(SOURCES
    octree.cpp
    octreebuilder.cpp
    octree_impl.cpp
    octreenode.cpp
    box.cpp
    linearoctree.cpp
    mortoncode.cpp
    octantid.cpp
    octree_utils.cpp
    paralleloctreebuilder.cpp
    perfinfo.cpp
    sequentialoctreebuilder.cpp
    vector3i.cpp
    vector_utils.cpp
)

set(PUBLIC_HEADER
    octree.h
    octreebuilder.h
    octreenode.h
    box.h
    octreebuilder_api.h
    paralleloctreebuilder.h
    sequentialoctreebuilder.h
    vector3i.h
    vector_utils.h
)

set(HEADER
    ${PUBLIC_HEADER}

    linearoctree.h
    logging.h
    mortoncode.h
    octantid.h
    octree_impl.h
    octree_utils.h
    parallel_stable_sort.h
    perfinfo.h
)

add_library(${target} ${LIBRARY_TYPE} ${HEADER} ${SOURCES})


include_directories(
    ${PROJECT_BINARY_DIR}
)

include_directories(
    BEFORE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set_target_properties(${target}
    PROPERTIES
    LINKER_LANGUAGE             CXX
    FOLDER                      ""
    COMPILE_DEFINITIONS_DEBUG           "${DEFAULT_COMPILE_DEFS_DEBUG}"
    COMPILE_DEFINITIONS_RELEASE         "${DEFAULT_COMPILE_DEFS_RELEASE}"
    COMPILE_DEFINITIONS_RELWITHDEBINFO  "${DEFAULT_COMPILE_DEFS_RELWITHDEBINFO}"
    LINK_FLAGS_DEBUG                    "${DEFAULT_LINKER_FLAGS_DEBUG}"
    LINK_FLAGS_RELEASE                  "${DEFAULT_LINKER_FLAGS_RELEASE}"
    LINK_FLAGS_RELWITHDEBINFO           "${DEFAULT_LINKER_FLAGS_RELWITHDEBINFO}"
    STATIC_LIBRARY_FLAGS_DEBUG          "${DEFAULT_LIBRARIAN_FLAGS_DEBUG}"
    STATIC_LIBRARY_FLAGS_RELEASE        "${DEFAULT_LIBRARIAN_FLAGS_RELEASE}"
    STATIC_LIBRARY_FLAGS_RELWITHDEBINFO "${DEFAULT_LIBRARIAN_FLAGS_RELWITHDEBINFO}"
    DEBUG_POSTFIX               "d")


target_compile_options(${target} PRIVATE "${DEFAULT_COMPILE_FLAGS}")

IF (BUILD_TESTS)
    add_subdirectory(tests)
ENDIF()

add_definitions(-DOCTREEBUILDER_EXPORTS)
install_module(${target})
