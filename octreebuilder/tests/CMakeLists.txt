set(target octreebuilder-tests)

include_directories(
    ${GTEST_INCLUDE_DIR}
    ${GMOCK_INCLUDE_DIR}
)

set(sources
    ../octree.cpp
    ../octreebuilder.cpp
    ../octree_impl.cpp
    ../octreenode.cpp
    ../box.cpp
    ../linearoctree.cpp
    ../mortoncode.cpp
    ../octantid.cpp
    ../octree_utils.cpp
    ../paralleloctreebuilder.cpp
    ../sequentialoctreebuilder.cpp
    ../vector3i.cpp
    ../vector_utils.cpp

    octreebuildertest.cpp
    octreetest.cpp
    boxtest.cpp
    linearoctreetest.cpp  
    mortoncodetest.cpp  
    octantidtest.cpp  
    octree_utilstest.cpp
    vector_utilstest.cpp

    unorderedpairmatcher.hpp
)

add_executable(${target} ${sources})

add_test(${target} ${target})

set_target_properties(${target}
    PROPERTIES
    LINKER_LANGUAGE             CXX
    FOLDER                      ""
    COMPILE_DEFINITIONS_DEBUG   "${DEFAULT_COMPILE_DEFS_DEBUG}"
    COMPILE_DEFINITIONS_RELEASE "${DEFAULT_COMPILE_DEFS_RELEASE}"
    LINK_FLAGS_DEBUG            "${DEFAULT_LINKER_FLAGS_DEBUG}"
    LINK_FLAGS_RELEASE          "${DEFAULT_LINKER_FLAGS_RELEASE}"  
    DEBUG_POSTFIX               "") # No debug postfix so that gtest runner for visual studio finds the test in debug mode


target_link_libraries(${target}
    gmock
    gmock_main
    ${TCMALLOC_LIBRARIES}
)


target_compile_options(${target} PRIVATE ${LINUX_TEST_COMPILE_FLAGS})

