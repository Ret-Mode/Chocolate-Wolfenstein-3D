add_executable(WolfensteinDemo
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
   ${backendFiles}
   )

target_compile_definitions(WolfensteinDemo PRIVATE GOODTIMES)
target_compile_definitions(WolfensteinDemo PRIVATE CARMACIZED)
target_compile_definitions(WolfensteinDemo PRIVATE UPLOAD)

target_include_directories(WolfensteinDemo PRIVATE ${OPENGL_INCLUDE_DIRS})
target_link_libraries(WolfensteinDemo ${OPENGL_LIBRARIES})

target_include_directories(WolfensteinDemo PRIVATE ${backendIncludes})
target_link_libraries(WolfensteinDemo ${backendLibs})

if (WIN32)
   set_target_properties(WolfensteinDemo PROPERTIES FOLDER ${CMAKE_CURRENT_BINARY_DIR})
endif ()

target_include_directories(WolfensteinDemo PRIVATE "include")
if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
   target_compile_options(WolfensteinDemo PRIVATE 
   -Wpointer-arith
   -Wreturn-type
   -Wwrite-strings
   -Wcast-align
   -Wsequence-point)
endif()



if (WIN32)
   target_link_libraries(WolfensteinDemo legacy_stdio_definitions.lib)
endif()

if (NOT WIN32)
   add_custom_command(TARGET WolfensteinDemo POST_BUILD 
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

   set_target_properties(WolfensteinDemo PROPERTIES FOLDER ${CMAKE_CURRENT_BINARY_DIR})
endif ()

