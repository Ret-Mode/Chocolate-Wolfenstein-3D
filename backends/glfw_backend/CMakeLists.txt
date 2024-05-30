
if(WIN32)
  set(GFLW3_INCLUDES "${CMAKE_SOURCE_DIR}\\glfw\\glfw\\include" CACHE PATH "glfw includes")
  set(GFLW3_LIBS "${CMAKE_SOURCE_DIR}\\glfw\\glfw\\lib-vc2022\\glfw3dll.lib" CACHE FILEPATH "glfw libs")
  set(GLAD_FILE "${CMAKE_SOURCE_DIR}\\glfw\\glad\\src\\glad.c" CACHE FILEPATH "glad c file")
  set(GLAD_INCLUDES "${CMAKE_SOURCE_DIR}\\glfw\\glad\\include" CACHE PATH "glad includes")
else()
  set(GFLW3_INCLUDES "")
  set(GFLW3_LIBS "glfw")
  set(GLAD_FILE "${CMAKE_SOURCE_DIR}/glfw/glad/src/glad.c" CACHE FILEPATH "glad c file")
  set(GLAD_INCLUDES "${CMAKE_SOURCE_DIR}/glfw/glad/include" CACHE PATH "glad includes")
  find_package(glfw3 REQUIRED)
endif()

find_package(OpenGL REQUIRED)

set(bFiles
  ${CMAKE_CURRENT_SOURCE_DIR}/src/sdl_keys.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/miniaudio_impl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/glfw_impl.cpp
  ${GLAD_FILE}
  PARENT_SCOPE)

set(bIncludes
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${OPENGL_INCLUDE_DIRS}
  ${GFLW3_INCLUDES}
  ${GLAD_INCLUDES}
  PARENT_SCOPE)

set(bLibs
  ${OPENGL_LIBRARIES}
  ${GFLW3_LIBS}
  PARENT_SCOPE)
