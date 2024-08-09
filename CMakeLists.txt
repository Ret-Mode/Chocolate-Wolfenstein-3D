cmake_minimum_required(VERSION 3.20)
project(Candy_Wolfenstein_3D VERSION 0.1.1 LANGUAGES C CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/data")

option(BUILD_WOLF_DEMO "Build Wolfenstein demo" ON)
option(BUILD_WOLF_FULL "Build Wolfenstein" OFF)
option(BUILD_SPEAR_DEMO "Build Spear demo" OFF)
option(BUILD_SPEAR_FULL "Build Spear" OFF)
option(BUILD_TESTS "Build Tests" ON)

add_subdirectory(backends)

function(build targetName sourceRoot backendFiles backendIncludes backendLibs defines doxygenFile)
    add_executable(${targetName}
        ${sourceRoot}/src/fmopl.cpp
        ${sourceRoot}/src/id_ca.cpp  
        ${sourceRoot}/src/id_in.cpp  
        ${sourceRoot}/src/id_pm.cpp  
        ${sourceRoot}/src/id_sd.cpp  
        ${sourceRoot}/src/id_us_1.cpp    
        ${sourceRoot}/src/id_vh.cpp  
        ${sourceRoot}/src/id_vl.cpp  
        ${sourceRoot}/src/signon.cpp  
        ${sourceRoot}/src/wl_act1.cpp  
        ${sourceRoot}/src/wl_act2.cpp  
        ${sourceRoot}/src/wl_agent.cpp  
        ${sourceRoot}/src/wl_debug.cpp  
        ${sourceRoot}/src/wl_draw.cpp  
        ${sourceRoot}/src/wl_game.cpp  
        ${sourceRoot}/src/wl_inter.cpp  
        ${sourceRoot}/src/wl_main.cpp   
        ${sourceRoot}/src/wl_menu.cpp  
        ${sourceRoot}/src/wl_state.cpp
        ${sourceRoot}/src/wl_play.cpp  
        ${sourceRoot}/src/wl_text.cpp
        ${${backendFiles}}
    )

    target_compile_definitions(${targetName} PRIVATE ${${defines}})

    target_include_directories(${targetName} PRIVATE ${${backendIncludes}})
    target_link_libraries(${targetName} ${${backendLibs}})

    if (WIN32)
        set_target_properties(${targetName} PROPERTIES FOLDER ${CMAKE_CURRENT_BINARY_DIR})
    endif ()

    target_include_directories(${targetName} PRIVATE ${sourceRoot}/include)
    if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        target_compile_options(${targetName} PRIVATE 
        -Wpointer-arith
        -Wreturn-type
        -Wwrite-strings
        -Wcast-align
        -Wsequence-point)
    endif()

    if (WIN32)
        target_link_libraries(${targetName} legacy_stdio_definitions.lib)
    endif()

    if (NOT WIN32)
    add_custom_command(TARGET ${targetName} POST_BUILD 
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

    endif ()

    if (buildDoxygen)
        find_path(DOXYGEN_PATH doxygen REQUIRED)
        add_custom_target(documentation${doxygenFile} COMMAND ${DOXYGEN_PATH} ${sourceRoot}/${doxygenFile} WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
        add_dependencies(${targetName} documentation${doxygenFile})
    endif()

endfunction()

set(buildDoxygen ON CACHE BOOL "build doxygen")

if (BUILD_WOLF_DEMO)
    set(wolfDemoDefines GOODTIMES CARMACIZED UPLOAD)
    if (BUILD_GLFW)
        build(WolfensteinDemoGLFW WolfensteinDemo glfwFiles glfwIncludes glfwLibs wolfDemoDefines doxygenGlfw)
    endif()
    if (BUILD_SDL)
        build(WolfensteinDemoSDL WolfensteinDemo sdlFiles sdlIncludes sdlLibs wolfDemoDefines doxygenSdl)
    endif()
endif()

if (BUILD_WOLF_FULL)
    set(wolfFullDefines GOODTIMES CARMACIZED)
    if (BUILD_GLFW)
        build(WolfensteinGLFW WolfensteinFull glfwFiles glfwIncludes glfwLibs wolfFullDefines doxygenGlfw)
    endif()
    if (BUILD_SDL)
        build(WolfensteinSDL WolfensteinFull sdlFiles sdlIncludes sdlLibs wolfFullDefines doxygenSdl)
    endif()
endif()

if (BUILD_SPEAR_DEMO)
    set(spearDemoDefines SPEAR SPEARDEMO GOODTIMES CARMACIZED)
    if (BUILD_GLFW)
        build(SpearOfDestinyDemoGLFW SpearDemo glfwFiles glfwIncludes glfwLibs spearDemoDefines doxygenGlfw)
    endif()
    if (BUILD_SDL)
        build(SpearOfDestinyDemoSDL SpearDemo sdlFiles sdlIncludes sdlLibs spearDemoDefines doxygenSdl)
    endif()
endif()

if (BUILD_SPEAR_FULL)
    set(spearFullDefines SPEAR GOODTIMES CARMACIZED)
    if (BUILD_GLFW)
        build(SpearOfDestinyGLFW SpearFull glfwFiles glfwIncludes glfwLibs spearFullDefines doxygenGlfw)
    endif()
    if (BUILD_SDL)
        build(SpearOfDestinySDL SpearFull sdlFiles sdlIncludes sdlLibs spearFullDefines doxygenSdl)
    endif()

endif()

if (BUILD_TESTS)
    add_subdirectory(tests)
endif()
