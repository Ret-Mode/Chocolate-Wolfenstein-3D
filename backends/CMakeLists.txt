message(${CMAKE_SOURCE_DIR})
message(${CMAKE_CURRENT_SOURCE_DIR})

set(backend "glfw-backend" CACHE STRING
  "graphics backend")

set_property(CACHE backend PROPERTY STRINGS "glfw-backend" "sdl-backend" )

# only when building with SDL
if(backend STREQUAL "sdl-backend")
   set(backendFiles
      ../sdl_backend/src/crt.cpp
      ../sdl_backend/src/sdl_music.cpp
      ../sdl_backend/src/sdl_graphics.cpp
      ../sdl_backend/src/sdl_keys.cpp
      )
    
    set(backendIncludes
        ../sdl_backend/include
        ${SDL_INCLUDE_DIR}
        ${SDL_MIXER_INCLUDE_DIR}
      )

    set(backendLibs
      ${SDL_LIBRARIES}
      ${SDL_MIXER_LIBRARIES}
    )
elseif(backend STREQUAL "glfw-backend")
    set(backendFiles
    ../glfw_backend/src/crt.cpp
    ../glfw_backend/src/sdl_graphics.cpp
    ../glfw_backend/src/sdl_keys.cpp
    ../glfw_backend/src/miniaudio_impl.cpp
    )

    set(backendIncludes
    ../glfw_backend/include
    ${SDL_INCLUDE_DIR}
    ${SDL_MIXER_INCLUDE_DIR}
    )

    set(backendLibs
    ${SDL_LIBRARIES}
    ${SDL_MIXER_LIBRARIES}
    )
endif()
