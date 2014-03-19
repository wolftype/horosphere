
#osc cmake file

cmake_minimum_required(VERSION 2.0)
cmake_policy(SET CMP0003 NEW)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -ftemplate-depth-1200 -Wno-switch -Wno-int-to-pointer-cast")

message("demo name: " ${SRC} )
get_filename_component(target ${SRC} NAME_WE)
message("target name: " ${target} )
add_executable(${target} ${SRC} )
target_link_libraries(${target} allocore alloutil ${ALLOCORE_LINK_LIBRARIES} ${ALLOUTIL_LINK_LIBRARIES} ${ALLOCORE_LINK_LIBRARIES} )
if(APPLE)
  set_target_properties(${target} PROPERTIES
  LINK_FLAGS "-pagezero_size 10000 -image_base 100000000")
endif()





