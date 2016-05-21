set(target benchmarks)

set(SOURCES
    octreebuilderbenchmark.cpp
)

include_directories(
    ${HAYAI_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}
)

add_executable(${target} ${SOURCES})

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

if(UNIX)
    target_compile_options(${target} PRIVATE
        ${DEFAULT_COMPILE_FLAGS}
        -Wno-reserved-id-macro
        -Wno-exit-time-destructors
        -Wno-global-constructors
        -Wno-weak-vtables)
else()
    target_compile_options(${target} PRIVATE ${DEFAULT_COMPILE_FLAGS})
endif()

target_link_libraries(${target}
    ${HAYAI_LIBRARIES}
    ${GPERFTOOLS_LIBRARIES}
    ${TCMALLOC_LIBRARIES}
    octreebuilder
)
