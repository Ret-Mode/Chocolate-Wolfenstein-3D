option(BUILD_GLFW "Build GLFW" ON)
option(BUILD_SDL "Build SDL" ON)

# only when building with SDL
if(BUILD_SDL)
  add_subdirectory(sdl_backend)
  set(sdlFiles ${subsdlFiles} PARENT_SCOPE)
  set(sdlIncludes ${subsdlIncludes} ${CMAKE_CURRENT_SOURCE_DIR}/include PARENT_SCOPE)
  set(sdlLibs ${subsdlLibs} PARENT_SCOPE)
endif()

if(BUILD_GLFW)
  add_subdirectory(glfw_backend)
  set(glfwFiles ${subglfwFiles} PARENT_SCOPE)
  set(glfwIncludes ${subglfwIncludes} ${CMAKE_CURRENT_SOURCE_DIR}/include PARENT_SCOPE)
  set(glfwLibs ${subglfwLibs} PARENT_SCOPE)
endif()