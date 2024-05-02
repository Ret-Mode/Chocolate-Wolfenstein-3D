cmake_minimum_required(VERSION 3.20)
project(Chocolate_Wolfenstein_3D VERSION 0.1.1 LANGUAGES C CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

option(BUILD_WOLF_DEMO "Build Wolfenstein demo" ON)
option(BUILD_WOLF_FULL "Build Wolfenstein" OFF)
option(BUILD_SPEAR_DEMO "Build Spear demo" OFF)
option(BUILD_SPEAR_FULL "Build Spear" OFF)

add_subdirectory(backends)

if (BUILD_WOLF_DEMO)
    add_subdirectory(WolfensteinDemo)
    target_link_libraries(WolfensteinDemo backendLib)
endif()

if (BUILD_WOLF_FULL)
    add_subdirectory(WolfensteinFull)
    target_link_libraries(WolfensteinFull backendLib)
endif()

if (BUILD_SPEAR_DEMO)
    add_subdirectory(SpearDemo)
    target_link_libraries(SpearDemo backendLib)
endif()

if (BUILD_SPEAR_FULL)
    add_subdirectory(SpearFull)
    target_link_libraries(SpearFull backendLib)
endif()
