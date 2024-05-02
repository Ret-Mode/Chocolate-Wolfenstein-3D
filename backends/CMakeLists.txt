message(${CMAKE_SOURCE_DIR})
message(${CMAKE_CURRENT_SOURCE_DIR})

set(backend "glfw-backend" CACHE STRING
  "graphics backend")

set_property(CACHE backend PROPERTY STRINGS "glfw-backend" "sdl-backend" )

if(backend STREQUAL "sdl-backend")
  add_subdirectory(sdl_backend)
  add_library(backendLib ALIAS backendLibSdl)
elseif(backend STREQUAL "glfw-backend")
  add_subdirectory(glfw_backend)
  add_library(backendLib ALIAS backendLibGlfw)
endif()
