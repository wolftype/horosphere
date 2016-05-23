cmake_minimum_required(VERSION 2.8)

cmake_policy(SET CMP0003 NEW)

#message("CMAKE_CURRENT_SOURCE_DIR: " ${CMAKE_CURRENT_SOURCE_DIR})
#message("CMAKE_SOURCE_DIR: " ${CMAKE_SOURCE_DIR})


#use clang on apple computers
if(APPLE)
option(USE_CLANG "use clang" ON)
endif()

if(USE_CLANG)
  message("using clang compiler")
  #   SET(CMAKE_CXX_COMPILER "clang++")
  #   SET(CMAKE_C_COMPILER "clang")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -stdlib=libc++ -ftemplate-depth-1200 -Wno-switch -Wno-int-to-pointer-cast " )
else()
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11  -ftemplate-depth-1200 -Wno-switch -Wno-int-to-pointer-cast")
endif()

#get rid of glut warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")

#one file
message("demo name: " ${SRC} )
get_filename_component(target ${SRC} NAME_WE)
add_executable(${target} ${SRC} )
target_link_libraries(${target} vsr vsrGfx gfx GLV Gamma allocore alloutil alloGLV ${ALLOCORE_LINK_LIBRARIES} ${ALLOUTIL_LINK_LIBRARIES} ${ALLOCORE_LINK_LIBRARIES} )
if(APPLE)
  set_target_properties(${target} PROPERTIES
  LINK_FLAGS "-pagezero_size 10000 -image_base 100000000")
endif()

#-pagezero_size 10000

#  file(GLOB DEMOS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)
#  foreach(demo ${DEMOS})
#
#    #NAME OF EXECUTABLE
#    get_filename_component(target ${demo} NAME_WE)
#    message("demo name: " ${demo} " target name: " ${target} )
#
#    #WHERE TO SAVE IT
#    set(CMAKE_EXEC_DIRECTORY ${CMAKE_SOURCE_DIR}/build/bin/examples/)
#
#    #ADD TO LIST AND LINK
#    add_executable(${target} ${demo} )
#    target_link_libraries(${target} vsr GLV ${ALLOCORE_LINK_LIBRARIES} ${ALLOUTIL_LINK_LIBRARIES} alloutil alloGLV )
#   
#    #DO THIS OR THERE WILL BE A SEG FAULT
#    if(APPLE)
#      set_target_properties(${target} PROPERTIES
#      LINK_FLAGS "-pagezero_size 10000 -image_base 100000000")
#    endif()
#    if(ALLOSPHERE)
#      set_target_properties(${target} PROPERTIES
#      LINK_FLAGS "-L/usr/lib/x86_64-linux-gnu/")
#    endif()
#
#  endforeach(demo)

