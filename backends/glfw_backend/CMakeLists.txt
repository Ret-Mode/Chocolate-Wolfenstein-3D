message(${CMAKE_SOURCE_DIR})
message(${CMAKE_CURRENT_SOURCE_DIR})

find_package(SDL REQUIRED)
find_package(SDL_mixer REQUIRED)

add_library(backendLib STATIC
            "${CMAKE_CURRENT_SOURCE_DIR}/src/crt.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/miniaudio_impl.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/sdl_graphics.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/sdl_keys.cpp")

target_include_directories(backendLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(backendLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)