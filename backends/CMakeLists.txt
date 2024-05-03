set(backend "glfw-backend" CACHE STRING
  "graphics backend")

set_property(CACHE backend PROPERTY STRINGS "glfw-backend" "sdl-backend" )

# only when building with SDL
if(backend STREQUAL "sdl-backend")
  add_subdirectory(sdl_backend)
elseif(backend STREQUAL "glfw-backend")
  add_subdirectory(glfw_backend)
endif()

set(backendFiles ${bFiles} PARENT_SCOPE)
set(backendIncludes ${bIncludes} ${CMAKE_CURRENT_SOURCE_DIR}/include PARENT_SCOPE)
set(backendLibs ${bLibs} PARENT_SCOPE)