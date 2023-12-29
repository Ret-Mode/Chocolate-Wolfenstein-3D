cmake_minimum_required(VERSION 3.13)
project(Chocolate_Wolfenstein_3D VERSION 0.1.0 LANGUAGES C CXX)

find_package(SDL REQUIRED)
find_package(SDL_mixer REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(Chocolate_Wolfenstein_3D
   src/crt.cpp
   src/fmopl.cpp
   src/id_ca.cpp  
   src/id_in.cpp  
   src/id_pm.cpp  
   src/id_sd.cpp  
   src/id_us_1.cpp    
   src/id_vh.cpp  
   src/id_vl.cpp  
   src/signon.cpp  
   src/wl_act1.cpp  
   src/wl_act2.cpp  
   src/wl_agent.cpp  
   src/wl_debug.cpp  
   src/wl_draw.cpp  
   src/wl_game.cpp  
   src/wl_inter.cpp  
   src/wl_main.cpp   
   src/wl_menu.cpp  
   src/wl_state.cpp
   src/wl_play.cpp  
   src/wl_text.cpp
   
   )

target_include_directories(Chocolate_Wolfenstein_3D PRIVATE "include")
if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
   target_compile_options(Chocolate_Wolfenstein_3D PRIVATE 
   -Wpointer-arith
   -Wreturn-type
   -Wwrite-strings
   -Wcast-align
   -Wsequence-point)
endif()

target_include_directories(Chocolate_Wolfenstein_3D PRIVATE SDL_INCLUDE_DIR)
target_include_directories(Chocolate_Wolfenstein_3D PRIVATE SDL_MIXER_INCLUDE_DIR)
add_custom_command(TARGET Chocolate_Wolfenstein_3D POST_BUILD 
   COMMAND cp ${CMAKE_SOURCE_DIR}/data/audiohed.wl1 ${CMAKE_CURRENT_BINARY_DIR}
   COMMAND cp ${CMAKE_SOURCE_DIR}/data/audiot.wl1 ${CMAKE_CURRENT_BINARY_DIR}
   COMMAND cp ${CMAKE_SOURCE_DIR}/data/config.wl1 ${CMAKE_CURRENT_BINARY_DIR}
   COMMAND cp ${CMAKE_SOURCE_DIR}/data/gamemaps.wl1 ${CMAKE_CURRENT_BINARY_DIR}
   COMMAND cp ${CMAKE_SOURCE_DIR}/data/maphead.wl1 ${CMAKE_CURRENT_BINARY_DIR}
   COMMAND cp ${CMAKE_SOURCE_DIR}/data/vgadict.wl1 ${CMAKE_CURRENT_BINARY_DIR}
   COMMAND cp ${CMAKE_SOURCE_DIR}/data/vgagraph.wl1 ${CMAKE_CURRENT_BINARY_DIR}
   COMMAND cp ${CMAKE_SOURCE_DIR}/data/vgahead.wl1 ${CMAKE_CURRENT_BINARY_DIR}
   COMMAND cp ${CMAKE_SOURCE_DIR}/data/vswap.wl1 ${CMAKE_CURRENT_BINARY_DIR}
   )
