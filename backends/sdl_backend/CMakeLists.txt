find_package(SDL REQUIRED)
find_package(SDL_mixer REQUIRED)
find_package(OpenGL REQUIRED)

add_library(backendLibSdl STATIC
            "${CMAKE_CURRENT_SOURCE_DIR}/src/crt.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/sdl_graphics.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/sdl_keys.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/sdl_music.cpp")

target_include_directories(backendLibSdl PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(backendLibSdl PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_include_directories(backendLibSdl PUBLIC ${OPENGL_INCLUDE_DIRS})
target_include_directories(backendLibSdl PUBLIC ${SDL_INCLUDE_DIR})
target_include_directories(backendLibSdl PUBLIC ${SDL_MIXER_INCLUDE_DIR})

target_link_libraries(backendLibSdl PUBLIC ${OPENGL_LIBRARIES})
target_link_libraries(backendLibSdl PUBLIC ${SDL_LIBRARIES})
target_link_libraries(backendLibSdl PUBLIC ${SDL_MIXER_LIBRARIES})
