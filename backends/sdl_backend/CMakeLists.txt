
find_package(SDL REQUIRED)
find_package(SDL_mixer REQUIRED)
find_package(OpenGL REQUIRED)


set(bFiles
  ${CMAKE_CURRENT_SOURCE_DIR}/src/crt.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/sdl_music.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/sdl_graphics.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/sdl_keys.cpp
  PARENT_SCOPE)

set(bIncludes
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${OPENGL_INCLUDE_DIRS}
  ${SDL_INCLUDE_DIR}
  ${SDL_MIXER_INCLUDE_DIR}
  PARENT_SCOPE)

set(bLibs
  ${OPENGL_LIBRARIES}
  ${SDL_LIBRARIES}
  ${SDL_MIXER_LIBRARY}
  ${SDLMAIN_LIBRARY}
  PARENT_SCOPE)