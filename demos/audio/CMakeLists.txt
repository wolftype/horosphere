#audio cmake file -- for building on bossanova

cmake_minimum_required(VERSION 2.8)
cmake_policy(SET CMP0003 NEW)

#OLD AUDIO
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -ftemplate-depth-1200 -Wno-switch -Wno-int-to-pointer-cast")


#use clang on apple computers
if(APPLE)
option(USE_CLANG "use clang" ON)
endif()

if(USE_CLANG)
  message("using clang compiler")
  SET(CMAKE_CXX_COMPILER "clang++")
  SET(CMAKE_C_COMPILER "clang")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++ -O3 -ftemplate-depth-1200 -Wno-switch -Wno-int-to-pointer-cast")
else()
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x  -O3 -ftemplate-depth-1200 -Wno-switch -Wno-int-to-pointer-cast")
endif()

#get rid of glut warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")


message("demo name: " ${SRC} )
get_filename_component(target ${SRC} NAME_WE)
message("target name: " ${target} )
add_executable(${target} ${SRC} )
target_link_libraries(${target} vsr vsrGfx gfx GLV Gamma allocore alloutil alloGLV ${ALLOCORE_LINK_LIBRARIES} ${ALLOUTIL_LINK_LIBRARIES} ${ALLOCORE_LINK_LIBRARIES} )
if(APPLE)
  set_target_properties(${target} PROPERTIES
  LINK_FLAGS "-pagezero_size 10000 -image_base 100000000")
endif()



