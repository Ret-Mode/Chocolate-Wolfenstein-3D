add_executable(SpearFull
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

target_compile_definitions(SpearFull PRIVATE SPEAR)
target_compile_definitions(SpearFull PRIVATE CARMACIZED)
target_compile_definitions(SpearFull PRIVATE GOODTIMES)

if (WIN32)
   set_target_properties(SpearFull PROPERTIES FOLDER ${CMAKE_CURRENT_BINARY_DIR})
endif ()

target_include_directories(SpearFull PRIVATE "include")
if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
   target_compile_options(SpearFull PRIVATE 
   -Wpointer-arith
   -Wreturn-type
   -Wwrite-strings
   -Wcast-align
   -Wsequence-point)
endif()

target_include_directories(SpearFull PRIVATE ${SDL_INCLUDE_DIR})
target_include_directories(SpearFull PRIVATE ${SDL_MIXER_INCLUDE_DIR})

target_include_directories(SpearFull PRIVATE ${OPENGL_INCLUDE_DIRS})

target_link_libraries(SpearFull ${SDL_LIBRARIES})
target_link_libraries(SpearFull ${SDL_MIXER_LIBRARIES})
target_link_libraries(SpearFull ${OPENGL_LIBRARIES})

if (WIN32)
   target_link_libraries(SpearFull legacy_stdio_definitions.lib)
endif()

if (NOT WIN32)
   add_custom_command(TARGET SpearFull POST_BUILD 
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
else()
   
   # configure_file(${CMAKE_SOURCE_DIR}/data/audiohed.wl1 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/audiohed.wl1 COPYONLY)
   # configure_file(${CMAKE_SOURCE_DIR}/data/audiot.wl1 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/audiot.wl1 COPYONLY)
   # configure_file(${CMAKE_SOURCE_DIR}/data/config.wl1 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/config.wl1 COPYONLY)
   # configure_file(${CMAKE_SOURCE_DIR}/data/gamemaps.wl1 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/gamemaps.wl1 COPYONLY)
   # configure_file(${CMAKE_SOURCE_DIR}/data/maphead.wl1 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/maphead.wl1 COPYONLY)
   # configure_file(${CMAKE_SOURCE_DIR}/data/vgadict.wl1 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vgadict.wl1 COPYONLY)
   # configure_file(${CMAKE_SOURCE_DIR}/data/vgagraph.wl1 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vgagraph.wl1 COPYONLY)
   # configure_file(${CMAKE_SOURCE_DIR}/data/vgahead.wl1 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vgahead.wl1 COPYONLY)
   # configure_file(${CMAKE_SOURCE_DIR}/data/vswap.wl1 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vswap.wl1 COPYONLY)
   # configure_file(${CMAKE_SOURCE_DIR}/SDL/lib/x86/SDL.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/SDL.dll COPYONLY)
   # configure_file(${CMAKE_SOURCE_DIR}/SDL_mixer/lib/SDL_mixer.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/SDL_mixer.dll COPYONLY)

   set_target_properties(SpearFull PROPERTIES FOLDER ${CMAKE_CURRENT_BINARY_DIR})
endif ()

