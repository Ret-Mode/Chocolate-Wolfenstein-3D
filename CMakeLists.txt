cmake_minimum_required(VERSION 3.20)
project(Chocolate_Wolfenstein_3D VERSION 0.1.1 LANGUAGES C CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

find_package(SDL REQUIRED)
find_package(SDL_mixer REQUIRED)
find_package(OpenGL REQUIRED)

option(BUILD_WOLF_DEMO "Build Wolfenstein demo" ON)
option(BUILD_WOLF_FULL "Build Wolfenstein" OFF)
option(BUILD_SPEAR_DEMO "Build Spear demo" OFF)
option(BUILD_SPEAR_FULL "Build Spear" OFF)

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
