cmake_minimum_required(VERSION 3.20)
project(Chocolate_Wolfenstein_3D VERSION 0.1.1 LANGUAGES C CXX)

message(${CMAKE_SOURCE_DIR})
message(${CMAKE_CURRENT_SOURCE_DIR})

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

find_package(SDL REQUIRED)
find_package(SDL_mixer REQUIRED)
find_package(OpenGL REQUIRED)

option(BUILD_WOLF_DEMO "Build Wolfenstein demo" ON)
option(BUILD_WOLF_FULL "Build Wolfenstein" OFF)
option(BUILD_SPEAR_DEMO "Build Spear demo" OFF)
option(BUILD_SPEAR_FULL "Build Spear" OFF)

# set(backend "glfw-backend" CACHE STRING
#   "graphics backend")

# set_property(CACHE backend PROPERTY STRINGS "glfw-backend" "sdl-backend" )

# # only when building with SDL
# if(backend STREQUAL "sdl-backend")
#    set(backendFiles
#       ../sdl_backend/src/crt.cpp
#       ../sdl_backend/src/sdl_music.cpp
#       ../sdl_backend/src/sdl_graphics.cpp
#       ../sdl_backend/src/sdl_keys.cpp
#       )
    
#     set(backendIncludes
#         ../sdl_backend/include
#         ${SDL_INCLUDE_DIR}
#         ${SDL_MIXER_INCLUDE_DIR}
#       )

#     set(backendLibs
#       ${SDL_LIBRARIES}
#       ${SDL_MIXER_LIBRARIES}
#     )
# elseif(backend STREQUAL "glfw-backend")
#     set(backendFiles
#     ../glfw_backend/src/crt.cpp
#     ../glfw_backend/src/sdl_graphics.cpp
#     ../glfw_backend/src/sdl_keys.cpp
#     ../glfw_backend/src/miniaudio_impl.cpp
#     )

#     set(backendIncludes
#     ../glfw_backend/include
#     ${SDL_INCLUDE_DIR}
#     ${SDL_MIXER_INCLUDE_DIR}
#     )

#     set(backendLibs
#     ${SDL_LIBRARIES}
#     ${SDL_MIXER_LIBRARIES}
#     )
# endif()


add_subdirectory(backends)


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
