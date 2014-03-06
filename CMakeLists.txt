#CMAKE FOR VERSOR+ALLOSPHERE DEMOS

cmake_minimum_required(VERSION 2.0)

project(HOROSPHERE)

set(ALLOSPHERE 0 CACHE STRING "build for allosphere")

set(BUILD_ALLOGLV 1)
set(NO_EXAMPLES 1)

#get linker info from AlloSystem make system, but do not make all examples
add_subdirectory(../AlloSystem AlloSystem)

#Was necessary to set_target_property of ALLOCORE_LINK_LIBRARIES within alloutil
#Would be nice to just grab AlloSystem Libraries (ALLOSYSTEM_LIBRARIES)
get_target_property(ALLOCORE_LINK_LIBRARIES allocore ALLOCORE_LINK_LIBRARIES)
get_target_property(ALLOUTIL_LINK_LIBRARIES alloutil ALLOUTIL_LINK_LIBRARIES)

#print out linker libs
message("ALLOCORE_LINK_LIBRARIES: " ${ALLOCORE_LINK_LIBRARIES})
message("ALLOUTIL_LINK_LIBRARIES: " ${ALLOUTIL_LINK_LIBRARIES})

#local include for this particular project space
include_directories(${PROJECT_SOURCE_DIR}/include/)
include_directories(${PROJECT_SOURCE_DIR}/../versor/)
include_directories(${PROJECT_SOURCE_DIR}/../versor/vsr/)
include_directories(${PROJECT_SOURCE_DIR}/../versor/ext/glv/)
include_directories(${PROJECT_SOURCE_DIR}/../versor/ext/gfx/)
include_directories(${PROJECT_SOURCE_DIR}/../Gamma/)

#would be nice to just grab AlloSystem Include Directories (ALLOSYSTEM_INCLUDE_DIR)
include_directories(${AlloSystem_SOURCE_DIR}/allocore/)
include_directories(${AlloSystem_SOURCE_DIR}/alloutil/)
include_directories(${AlloSystem_SOURCE_DIR}/alloGLV/)

#library search directories
link_directories(${PROJECT_SOURCE_DIR}/../versor/build/lib/)
link_directories(${PROJECT_SOURCE_DIR}/../versor/ext/glv/build/lib/)
link_directories(${PROJECT_SOURCE_DIR}/../Gamma/build/lib/)
link_directories(${AlloSystem_SOURCE_DIR}/build/lib)

if(ALLOSPHERE)
link_directories(/usr/lib/x86_64-linux-gnu/)
endif()


get_directory_property(LD LINK_DIRECTORIES)
message("LINK_DIRECTORIES" ${LD})


#use clang on apple computers
if(APPLE)
option(USE_CLANG "use clang" ON)
endif()

if(USE_CLANG)
  message("using clang compiler")
  SET(CMAKE_CXX_COMPILER "clang++")
  SET(CMAKE_C_COMPILER "clang")
  SET(CMAKE_CXX_FLAGS "-std=c++11 -O3 -ftemplate-depth-1200 -Wno-switch -Wno-int-to-pointer-cast")
else()
  SET(CMAKE_CXX_FLAGS "-std=c++0x -O3 -ftemplate-depth-1200 -Wno-switch -Wno-int-to-pointer-cast")
endif()

if(ALLOSPHERE)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D __allosphere__")
endif()

#the subdirectory of demos
add_subdirectory(demos)

message("now type 'make'")
