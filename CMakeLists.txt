#CMAKE FOR VERSOR+ALLOSPHERE DEMOS

cmake_minimum_required(VERSION 2.8)

project(HOROSPHERE)

set(BUILD_ALLOGLV 1)
set(NO_EXAMPLES 1)

#get linker info from AlloSystem make system, but do not make all examples
add_subdirectory(../AlloSystem AlloSystem)

#NOTE: Was necessary to set_target_property of ALLOCORE_LINK_LIBRARIES within alloutil
#Would be nice to just grab AlloSystem Libraries (ALLOSYSTEM_LIBRARIES)
get_target_property(ALLOCORE_LINK_LIBRARIES allocore ALLOCORE_LINK_LIBRARIES)
get_target_property(ALLOUTIL_LINK_LIBRARIES alloutil ALLOUTIL_LINK_LIBRARIES)

#would be nice to just grab AlloSystem Include Directories (ALLOSYSTEM_INCLUDE_DIR)
include_directories(${AlloSystem_SOURCE_DIR}/allocore/)
include_directories(${AlloSystem_SOURCE_DIR}/alloutil/)
include_directories(${AlloSystem_SOURCE_DIR}/alloGLV/)

#local include for this particular project space
include_directories(/usr/local/include/)
include_directories(${PROJECT_SOURCE_DIR}/include/)
include_directories(${PROJECT_SOURCE_DIR}/demos/)
include_directories(${PROJECT_SOURCE_DIR}/../versor/include/)
include_directories(${PROJECT_SOURCE_DIR}/../versor/include/vsr/)
include_directories(${PROJECT_SOURCE_DIR}/../GLV/)

#change to point to gfx?
include_directories(${PROJECT_SOURCE_DIR}/../versor/ext/gfx/)
include_directories(${PROJECT_SOURCE_DIR}/../versor/ext/gfx/gfx/)
include_directories(${PROJECT_SOURCE_DIR}/../Gamma/)
include_directories(${PROJECT_SOURCE_DIR}/../cuttlebone/)
include_directories(${PROJECT_SOURCE_DIR}/../ohio/include)
include_directories(${PROJECT_SOURCE_DIR}/../ohio/ext/hana/include)


#library search directories
link_directories(/usr/local/lib/)
link_directories(${PROJECT_SOURCE_DIR}/../versor/build/)
link_directories(${PROJECT_SOURCE_DIR}/../versor/build/ext/gfx/)
link_directories(${PROJECT_SOURCE_DIR}/../GLV/build/lib/)
link_directories(${PROJECT_SOURCE_DIR}/../Gamma/build/lib/)
link_directories(${AlloSystem_SOURCE_DIR}/build/lib)

if(${BUILDING_FOR_ALLOSPHERE})
link_directories(/usr/lib/x86_64-linux-gnu/)
endif()


#get_directory_property(LD LINK_DIRECTORIES)
#message("LINK_DIRECTORIES" ${LD})

if(${DEBUGMODE})
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
else()
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
endif()

#COMPILER
if(${BUILDING_FOR_ALLOSPHERE})
  message( "ALLO BUILD")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D __allosphere__")
if(${BUILDING_FOR_LINUX})
  message( "LINUX (GR01) BUILD")
  # SET(CMAKE_CXX_COMPILER ${PROJECT_SOURCE_DIR}/../clang+llvm-3.8.0-x86_64-apple-darwin/bin/clang++)
else()
  message( "OSX BUILD")
  SET(CMAKE_CXX_COMPILER ${PROJECT_SOURCE_DIR}/../clang+llvm-3.8.0-x86_64-apple-darwin/bin/clang++)
endif()
else()
  message( "PABLO'S LAPTOP OSX BUILD")
  SET(CMAKE_CXX_COMPILER clang++)
endif()

#COMPILER FLAGS
if(${BUILDING_FOR_LINUX})
  message( "LINUX BUILD")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D AL_LINUX ")
else()
  message( "OSX BUILD")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D AL_OSX") #hack due to trouble with al_OpenGL.hpp 
endif()

#the subdirectory of demos
set( EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin )

#ADDS CMAKEFILELISTS FROM demos/graphics or demos/audio etc
add_subdirectory(${DIR})
message("now type 'make'")
