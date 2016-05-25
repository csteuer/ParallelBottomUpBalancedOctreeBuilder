set(target octreebuilder-unittests)

include_directories(
    ${GTEST_INCLUDE_DIR}
    ${GMOCK_INCLUDE_DIR}
)

set(sources
    octreetest.cpp
    boxtest.cpp
    linearoctreetest.cpp  
    mortoncode_utilstest.cpp
    octantidtest.cpp  
    octree_utilstest.cpp
    vector_utilstest.cpp
)

add_executable(${target} ${sources})

add_test(${target} ${target})

set_target_properties(${target}
    PROPERTIES
    LINKER_LANGUAGE             CXX
    FOLDER                      ""
    COMPILE_DEFINITIONS                 "${DEFAULT_COMPILE_DEFS}"
    COMPILE_DEFINITIONS_DEBUG           "${DEFAULT_COMPILE_DEFS_DEBUG}"
    COMPILE_DEFINITIONS_RELEASE         "${DEFAULT_COMPILE_DEFS_RELEASE}"
    COMPILE_DEFINITIONS_RELWITHDEBINFO  "${DEFAULT_COMPILE_DEFS_RELWITHDEBINFO}"
    LINK_FLAGS                          "${DEFAULT_LINKER_FLAGS}"
    LINK_FLAGS_DEBUG                    "${DEFAULT_LINKER_FLAGS_DEBUG}"
    LINK_FLAGS_RELEASE                  "${DEFAULT_LINKER_FLAGS_RELEASE}"
    LINK_FLAGS_RELWITHDEBINFO           "${DEFAULT_LINKER_FLAGS_RELWITHDEBINFO}"
    STATIC_LIBRARY_FLAGS_DEBUG          "${DEFAULT_LIBRARIAN_FLAGS_DEBUG}"
    STATIC_LIBRARY_FLAGS_RELEASE        "${DEFAULT_LIBRARIAN_FLAGS_RELEASE}"
    STATIC_LIBRARY_FLAGS_RELWITHDEBINFO "${DEFAULT_LIBRARIAN_FLAGS_RELWITHDEBINFO}"
    DEBUG_POSTFIX               "") # No debug postfix so that gtest runner for visual studio finds the test in debug mode


target_link_libraries(${target}
    octreebuilder
    gmock
    gmock_main
    ${TCMALLOC_LIBRARIES}
)


target_compile_options(${target} PRIVATE ${LINUX_TEST_COMPILE_FLAGS})

