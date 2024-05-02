find_package(SDL REQUIRED)
find_package(OpenGL REQUIRED)

add_library(backendLibGlfw STATIC
            "${CMAKE_CURRENT_SOURCE_DIR}/src/crt.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/miniaudio_impl.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/sdl_graphics.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/sdl_keys.cpp")

target_include_directories(backendLibGlfw PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(backendLibGlfw PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_include_directories(backendLibGlfw PUBLIC ${OPENGL_INCLUDE_DIRS})
target_include_directories(backendLibGlfw PUBLIC ${SDL_INCLUDE_DIR})

target_link_libraries(backendLibGlfw PUBLIC ${OPENGL_LIBRARIES})
target_link_libraries(backendLibGlfw PUBLIC ${SDL_LIBRARIES})