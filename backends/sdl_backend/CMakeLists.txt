
find_package(SDL REQUIRED)
find_package(SDL_mixer REQUIRED)
find_package(OpenGL REQUIRED)


set(subsdlFiles
  ${CMAKE_CURRENT_SOURCE_DIR}/src/crt.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/sdl_music.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/sdl_graphics.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/sdl_keys.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/wl_draw_backend.cpp
  PARENT_SCOPE)

set(subsdlIncludes
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${OPENGL_INCLUDE_DIRS}
  ${SDL_INCLUDE_DIR}
  ${SDL_MIXER_INCLUDE_DIR}
  PARENT_SCOPE)

set(subsdlLibs
  ${OPENGL_LIBRARIES}
  ${SDL_LIBRARIES}
  ${SDL_MIXER_LIBRARY}
  ${SDLMAIN_LIBRARY}
  PARENT_SCOPE)