if (BUILD_GLFW)
   add_executable(WolfensteinDemoGLFW
      ${CMAKE_CURRENT_SOURCE_DIR}/src/fmopl.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/src/id_ca.cpp  
      ${CMAKE_CURRENT_SOURCE_DIR}/src/id_in.cpp  
      ${CMAKE_CURRENT_SOURCE_DIR}/src/id_pm.cpp  
      ${CMAKE_CURRENT_SOURCE_DIR}/src/id_sd.cpp  
      ${CMAKE_CURRENT_SOURCE_DIR}/src/id_us_1.cpp    
      ${CMAKE_CURRENT_SOURCE_DIR}/src/id_vh.cpp  
      ${CMAKE_CURRENT_SOURCE_DIR}/src/id_vl.cpp  
      ${CMAKE_CURRENT_SOURCE_DIR}/src/signon.cpp  
      ${CMAKE_CURRENT_SOURCE_DIR}/src/wl_act1.cpp  
      ${CMAKE_CURRENT_SOURCE_DIR}/src/wl_act2.cpp  
      ${CMAKE_CURRENT_SOURCE_DIR}/src/wl_agent.cpp  
      ${CMAKE_CURRENT_SOURCE_DIR}/src/wl_debug.cpp  
      ${CMAKE_CURRENT_SOURCE_DIR}/src/wl_draw.cpp  
      ${CMAKE_CURRENT_SOURCE_DIR}/src/wl_game.cpp  
      ${CMAKE_CURRENT_SOURCE_DIR}/src/wl_inter.cpp  
      ${CMAKE_CURRENT_SOURCE_DIR}/src/wl_main.cpp   
      ${CMAKE_CURRENT_SOURCE_DIR}/src/wl_menu.cpp  
      ${CMAKE_CURRENT_SOURCE_DIR}/src/wl_state.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/src/wl_play.cpp  
      ${CMAKE_CURRENT_SOURCE_DIR}/src/wl_text.cpp
      ${glfwFiles}
      )

   target_compile_definitions(WolfensteinDemoGLFW PRIVATE GOODTIMES)
   target_compile_definitions(WolfensteinDemoGLFW PRIVATE CARMACIZED)
   target_compile_definitions(WolfensteinDemoGLFW PRIVATE UPLOAD)

   target_include_directories(WolfensteinDemoGLFW PRIVATE ${glfwIncludes})
   target_link_libraries(WolfensteinDemoGLFW ${glfwLibs})

   if (WIN32)
      set_target_properties(WolfensteinDemoGLFW PROPERTIES FOLDER ${CMAKE_CURRENT_BINARY_DIR})
   endif ()

   target_include_directories(WolfensteinDemoGLFW PRIVATE "include")
   if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
      target_compile_options(WolfensteinDemoGLFW PRIVATE 
      -Wpointer-arith
      -Wreturn-type
      -Wwrite-strings
      -Wcast-align
      -Wsequence-point)
   endif()

   if (WIN32)
      target_link_libraries(WolfensteinDemoGLFW legacy_stdio_definitions.lib)
   endif()

   if (NOT WIN32)
      add_custom_command(TARGET WolfensteinDemoGLFW POST_BUILD 
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
      
      # add_custom_command(TARGET WolfensteinDemo POST_BUILD 
      # COMMAND "copy ${CMAKE_SOURCE_DIR}\\data\\audiohed.wl1 ${CMAKE_CURRENT_BINARY_DIR}"
      # COMMAND "copy ${CMAKE_SOURCE_DIR}\\data\\audiot.wl1 ${CMAKE_CURRENT_BINARY_DIR}"
      # COMMAND "copy ${CMAKE_SOURCE_DIR}\\data\\config.wl1 ${CMAKE_CURRENT_BINARY_DIR}"
      # COMMAND "copy ${CMAKE_SOURCE_DIR}\\data\\gamemaps.wl1 ${CMAKE_CURRENT_BINARY_DIR}"
      # COMMAND "copy ${CMAKE_SOURCE_DIR}\\data\\maphead.wl1 ${CMAKE_CURRENT_BINARY_DIR}"
      # COMMAND "copy ${CMAKE_SOURCE_DIR}\\data\\vgadict.wl1 ${CMAKE_CURRENT_BINARY_DIR}"
      # COMMAND "copy ${CMAKE_SOURCE_DIR}\\data\\vgagraph.wl1 ${CMAKE_CURRENT_BINARY_DIR}"
      # COMMAND "copy ${CMAKE_SOURCE_DIR}\\data\\vgahead.wl1 ${CMAKE_CURRENT_BINARY_DIR}"
      # COMMAND "copy ${CMAKE_SOURCE_DIR}\\data\\vswap.wl1 ${CMAKE_CURRENT_BINARY_DIR}")
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

      #set_target_properties(WolfensteinDemo PROPERTIES FOLDER ${CMAKE_CURRENT_BINARY_DIR})
   endif ()

   if (buildDoxygen)
      find_path(DOXYGEN_PATH doxygen REQUIRED)
      add_custom_target(documentation COMMAND ${DOXYGEN_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/doxygenGlfw WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
      add_dependencies(WolfensteinDemoGLFW documentation)
   endif()
endif()

if (BUILD_SDL)
   add_executable(WolfensteinDemoSDL
      ${CMAKE_CURRENT_SOURCE_DIR}/src/fmopl.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/src/id_ca.cpp  
      ${CMAKE_CURRENT_SOURCE_DIR}/src/id_in.cpp  
      ${CMAKE_CURRENT_SOURCE_DIR}/src/id_pm.cpp  
      ${CMAKE_CURRENT_SOURCE_DIR}/src/id_sd.cpp  
      ${CMAKE_CURRENT_SOURCE_DIR}/src/id_us_1.cpp    
      ${CMAKE_CURRENT_SOURCE_DIR}/src/id_vh.cpp  
      ${CMAKE_CURRENT_SOURCE_DIR}/src/id_vl.cpp  
      ${CMAKE_CURRENT_SOURCE_DIR}/src/signon.cpp  
      ${CMAKE_CURRENT_SOURCE_DIR}/src/wl_act1.cpp  
      ${CMAKE_CURRENT_SOURCE_DIR}/src/wl_act2.cpp  
      ${CMAKE_CURRENT_SOURCE_DIR}/src/wl_agent.cpp  
      ${CMAKE_CURRENT_SOURCE_DIR}/src/wl_debug.cpp  
      ${CMAKE_CURRENT_SOURCE_DIR}/src/wl_draw.cpp  
      ${CMAKE_CURRENT_SOURCE_DIR}/src/wl_game.cpp  
      ${CMAKE_CURRENT_SOURCE_DIR}/src/wl_inter.cpp  
      ${CMAKE_CURRENT_SOURCE_DIR}/src/wl_main.cpp   
      ${CMAKE_CURRENT_SOURCE_DIR}/src/wl_menu.cpp  
      ${CMAKE_CURRENT_SOURCE_DIR}/src/wl_state.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/src/wl_play.cpp  
      ${CMAKE_CURRENT_SOURCE_DIR}/src/wl_text.cpp
      ${sdlFiles}
      )

   target_compile_definitions(WolfensteinDemoSDL PRIVATE GOODTIMES)
   target_compile_definitions(WolfensteinDemoSDL PRIVATE CARMACIZED)
   target_compile_definitions(WolfensteinDemoSDL PRIVATE UPLOAD)

   target_include_directories(WolfensteinDemoSDL PRIVATE ${sdlIncludes})
   target_link_libraries(WolfensteinDemoSDL ${sdlLibs})

   if (WIN32)
      set_target_properties(WolfensteinDemoSDL PROPERTIES FOLDER ${CMAKE_CURRENT_BINARY_DIR})
   endif ()

   target_include_directories(WolfensteinDemoSDL PRIVATE "include")
   if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
      target_compile_options(WolfensteinDemoSDL PRIVATE 
      -Wpointer-arith
      -Wreturn-type
      -Wwrite-strings
      -Wcast-align
      -Wsequence-point)
   endif()

   if (WIN32)
      target_link_libraries(WolfensteinDemoSDL legacy_stdio_definitions.lib)
   endif()

   if (NOT WIN32)
      add_custom_command(TARGET WolfensteinDemoSDL POST_BUILD 
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
      
      # add_custom_command(TARGET WolfensteinDemo POST_BUILD 
      # COMMAND "copy ${CMAKE_SOURCE_DIR}\\data\\audiohed.wl1 ${CMAKE_CURRENT_BINARY_DIR}"
      # COMMAND "copy ${CMAKE_SOURCE_DIR}\\data\\audiot.wl1 ${CMAKE_CURRENT_BINARY_DIR}"
      # COMMAND "copy ${CMAKE_SOURCE_DIR}\\data\\config.wl1 ${CMAKE_CURRENT_BINARY_DIR}"
      # COMMAND "copy ${CMAKE_SOURCE_DIR}\\data\\gamemaps.wl1 ${CMAKE_CURRENT_BINARY_DIR}"
      # COMMAND "copy ${CMAKE_SOURCE_DIR}\\data\\maphead.wl1 ${CMAKE_CURRENT_BINARY_DIR}"
      # COMMAND "copy ${CMAKE_SOURCE_DIR}\\data\\vgadict.wl1 ${CMAKE_CURRENT_BINARY_DIR}"
      # COMMAND "copy ${CMAKE_SOURCE_DIR}\\data\\vgagraph.wl1 ${CMAKE_CURRENT_BINARY_DIR}"
      # COMMAND "copy ${CMAKE_SOURCE_DIR}\\data\\vgahead.wl1 ${CMAKE_CURRENT_BINARY_DIR}"
      # COMMAND "copy ${CMAKE_SOURCE_DIR}\\data\\vswap.wl1 ${CMAKE_CURRENT_BINARY_DIR}")
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

      #set_target_properties(WolfensteinDemo PROPERTIES FOLDER ${CMAKE_CURRENT_BINARY_DIR})
   endif ()

   if (buildDoxygen)
      find_path(DOXYGEN_PATH doxygen REQUIRED)
      add_custom_target(documentation COMMAND ${DOXYGEN_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/doxygenSdl WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
      add_dependencies(WolfensteinDemoSDL documentation)
   endif()
endif()