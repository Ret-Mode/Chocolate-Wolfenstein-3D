cmake_minimum_required(VERSION 3.20)
project(Candy_Wolfenstein_3D VERSION 0.1.1 LANGUAGES C CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/data")

option(BUILD_WOLF_DEMO "Build Wolfenstein demo" ON)
option(BUILD_WOLF_FULL "Build Wolfenstein" OFF)
option(BUILD_SPEAR_DEMO "Build Spear demo" OFF)
option(BUILD_SPEAR_FULL "Build Spear" OFF)
option(BUILD_TESTS "Build Tests" ON)

add_subdirectory(backends)

set(buildDoxygen ON CACHE BOOL "build doxygen")

if (BUILD_WOLF_DEMO)
    add_subdirectory(WolfensteinDemo)
endif()

if (BUILD_WOLF_FULL)
    add_subdirectory(WolfensteinFull)
endif()

if (BUILD_SPEAR_DEMO)
    add_subdirectory(SpearDemo)
endif()

if (BUILD_SPEAR_FULL)
    add_subdirectory(SpearFull)
endif()

if (BUILD_TESTS)
    add_subdirectory(tests)
endif()
