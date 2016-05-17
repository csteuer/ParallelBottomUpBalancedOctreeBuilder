set(target benchmarks)

set(SOURCES
    octreebuilderbenchmark.cpp
)

include_directories(
    ${HAYAI_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/octreebuilder/include
)

add_executable(${target} ${SOURCES})

target_link_libraries(${target}
    ${HAYAI_LIBRARIES}
    octreebuilder
)
