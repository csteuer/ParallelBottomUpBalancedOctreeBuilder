cmake_minimum_required(VERSION 2.8.12)

cmake_policy(SET CMP0020 NEW)
cmake_policy(SET CMP0043 OLD) # Allow definitions of compile flags via COMPILE_DEFINITIONS_<CONFIG> target properties
cmake_policy(SET CMP0028 OLD) # Allow colons in target names

set(PROJECTNAME ParallelBottomUpBalancedOctreeBuilder)

project(${PROJECTNAME})

set(${PROJECTNAME}_VERSION_MAJOR 0)
set(${PROJECTNAME}_VERSION_MINOR 1)
set(${PROJECTNAME}_VERSION_PATCH 0)
set(${PROJECTNAME}_VERSION ${${PROJECTNAME}_VERSION_MAJOR}.${${PROJECTNAME}_VERSION_MINOR}.${${PROJECTNAME}_VERSION_PATCH})


##########################################################################################################
# Environment Setup
##########################################################################################################

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)


# Setup platform specifics (compile flags, etc., ...)
if(MSVC)
    include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/PlatformWindowsMSVC.cmake)
elseif(UNIX)
    include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/PlatformLinux.cmake)
else()
    message(WARNING "Unsupported platform/compiler combination")
endif()

# standard naming scheme
set(CMAKE_DEBUG_POSTFIX "_debug")
set(CMAKE_RELWITHDEBINFO_POSTFIX "_rdebug")
set(CMAKE_MINSIZEREL_POSTFIX "_small")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")

find_package(Doxygen)

##########################################################################################################
# GTEST
##########################################################################################################

option(BUILD_TESTS "Builds the tests" OFF)

IF(BUILD_TESTS)
	SET(gtest_force_shared_crt TRUE) # For Visual C++: Force Gtest to link against shared runtime library (so that we can compile with /MD)
	add_definitions("-DGTEST_LANG_CXX11")
	add_subdirectory(googletest)
	enable_testing()

    if (UNIX)
        # disable/ignore compiler warnings for googletest
       set(DEFAULT_COMPILE_FLAGS  -isystem${gmock_SOURCE_DIR}/include -isystem${gtest_SOURCE_DIR}/include ${DEFAULT_COMPILE_FLAGS})
    endif()

    include_directories(${gmock_SOURCE_DIR}/include ${gmock_SOURCE_DIR} ${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
ENDIF()

##########################################################################################################
# Profiling & Performance optimization
##########################################################################################################

option(ENABLE_PROFILING "Links against google performance tools (Gperftools) to enable profiling." OFF)
if (ENABLE_PROFILING)
    find_package(Gperftools REQUIRED)
    include_directories(GPERFTOOLS_INCLUDE_DIR)
    add_definitions(-DPROFILING_ENABLED)
endif()

option(BUILD_WITH_TCMALLOC "Uses googles TCMALLOC (which is faster than malloc) if found." ON)
if ((NOT ENABLE_PROFILING) AND BUILD_WITH_TCMALLOC)
    # If profiling is enabled TCMALLOC is linked anyway.
    # In contrast to find_package(Gperftools REQUIRED) this will find the minimal tcmalloc version (without heap checker and profiler)
    find_package(TCMALLOC)
    if (NOT TCMALLOC_FOUND)
        set(TCMALLOC_LIBRARIES "")
        message(WARNING "BUILD_WITH_TCMALLOC is TRUE but could not find tcmalloc_minimal. Will use default malloc.")
    endif()
endif()

##########################################################################################################
# Benchmarks (with hayai)
##########################################################################################################

option(BUILD_BENCHMARKS "Builds the benchmark tests" OFF)

IF(BUILD_BENCHMARKS)
    find_package(hayai REQUIRED)

    if(${UNIX})
        foreach(path ${HAYAI_INCLUDE_DIRS})
          set(DEFAULT_COMPILE_FLAGS  -isystem${path} ${DEFAULT_COMPILE_FLAGS})
          set(LINUX_TEST_COMPILE_FLAGS  -isystem${path} ${LINUX_TEST_COMPILE_FLAGS})
        endforeach()
    endif()

    add_subdirectory(benchmarks)
ENDIF()

##########################################################################################################
# Other options
##########################################################################################################

# Default cmake variable that is overwritten by gmock/gtest
option(BUILD_SHARED_LIBS "Build shared octreebuilder/gmock/gtest libraries (builds static libraries otherwise)." OFF)

##########################################################################################################
# Install definitions
##########################################################################################################

set(INSTALL_LIB_DIR "${CMAKE_INSTALL_PREFIX}/lib")
set(INSTALL_BIN_DIR "${CMAKE_INSTALL_PREFIX}/bin")
set(INSTALL_INCLUDE_DIR "${CMAKE_INSTALL_PREFIX}/include")
set(INSTALL_CMAKE_DIR "${CMAKE_INSTALL_PREFIX}/cmake")

set(EXPORT_TARGETS "" CACHE INTERNAL "Installed Targets" FORCE)
set(API_HEADER "" CACHE INTERAL "The header files of the public api." FORCE)

function(install_module target)

    install(TARGETS ${target}
        EXPORT octreebuilderTargets
        ARCHIVE DESTINATION ${INSTALL_LIB_DIR}
        LIBRARY DESTINATION ${INSTALL_LIB_DIR} CONFIGURATIONS
        RUNTIME DESTINATION ${INSTALL_BIN_DIR} CONFIGURATIONS
        PUBLIC_HEADER DESTINATION ${INSTALL_INCLUDE_DIR}/octreebuilder CONFIGURATIONS
        COMPONENT dev)


    foreach(file ${PUBLIC_HEADER})
        install(FILES ${file} DESTINATION ${INSTALL_INCLUDE_DIR}/octreebuilder)
        set(API_HEADER ${API_HEADER} ${CMAKE_CURRENT_SOURCE_DIR}/${file} CACHE INTERNAL "The header files of the public api.")
    endforeach()

    get_target_property(type ${target} TYPE)
    if(${type} MATCHES "_LIBRARY$")
        set(EXPORT_TARGETS ${EXPORT_TARGETS} ${target} CACHE INTERNAL "Installed Targets")
    endif()

endfunction()

##########################################################################################################
# Sub directories
##########################################################################################################

add_subdirectory(octreebuilder)

#option(BUILD_APPS "Build the demo applications" OFF)
#if (BUILD_APPS)
#    add_subdirectory(apps)
#endif()

##########################################################################################################
# Install
##########################################################################################################

export(TARGETS ${EXPORT_TARGETS} FILE "${PROJECT_BINARY_DIR}/octreebuilderTargets.cmake")

file(RELATIVE_PATH REL_INCLUDE_DIR ${INSTALL_CMAKE_DIR} ${INSTALL_INCLUDE_DIR})

set(PACKAGE_VERSION ${PROJECTNAME}_VERSION)

# ... for the build tree
set(CONF_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}" "${PROJECT_BINARY_DIR}")
configure_file(octreebuilderConfig.cmake.in "${PROJECT_BINARY_DIR}/octreebuilderConfig.cmake" @ONLY)

# ... for the install tree
set(CONF_INCLUDE_DIRS "\${octreebuilder_CMAKE_DIR}/${REL_INCLUDE_DIR}")
configure_file(octreebuilderConfig.cmake.in "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/octreebuilderConfig.cmake" @ONLY)

# ... for both
configure_file(octreebuilderVersion.cmake.in "${PROJECT_BINARY_DIR}/octreebuilderVersion.cmake" @ONLY)

# Install the octreebuilderConfig.cmake and octreebuilderVersion.cmake
install(FILES
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/octreebuilderConfig.cmake"
  "${PROJECT_BINARY_DIR}/octreebuilderVersion.cmake"
  DESTINATION cmake COMPONENT dev)

# Install the export set for use with the install-tree
install(EXPORT octreebuilderTargets DESTINATION cmake COMPONENT dev)

##########################################################################################################
# Doxygen
##########################################################################################################
if(DOXYGEN_FOUND)
    set(DOXYGEN_INPUT "${API_HEADER}")
    set(MDFILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
    string (REPLACE ";" " " DOXYGEN_INPUT "${DOXYGEN_INPUT} ${MDFILE}")
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doxygen_conf.in ${CMAKE_CURRENT_BINARY_DIR}/doxygen_conf @ONLY)
    add_custom_target(doc
        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/doxygen_conf
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/doc
        COMMENT "Generating API documentation with Doxygen" VERBATIM
        SOURCES doxygen_conf.in README.md
    )
endif(DOXYGEN_FOUND)
