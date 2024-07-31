add_executable(packerTest
   ${CMAKE_CURRENT_SOURCE_DIR}/packerTest.cpp
   )

if (WIN32)
   set_target_properties(packerTest PROPERTIES FOLDER ${CMAKE_CURRENT_BINARY_DIR})
endif ()

if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
   target_compile_options(packerTest PRIVATE 
   -Wpointer-arith
   -Wreturn-type
   -Wwrite-strings
   -Wcast-align
   -Wsequence-point)
endif()

if (WIN32)
   target_link_libraries(packerTest legacy_stdio_definitions.lib)
endif()
