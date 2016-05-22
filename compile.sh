#!/bin/bash


# FLAGS:
# -a | --allosphere : build for allosphere
# -q | --quiet : make quietly (VERBOSE=0)
# 

echo Compiling $1 

DIRECTORY=`dirname $1`
TARGET=`basename $1|cut -d'.' -f1 | sed -e "s|/|_|g"`
FILENAME=${TARGET}.cpp
echo Directory is $DIRECTORY
echo Filename is $FILENAME
echo Target is $TARGET


#DEFAULTS
BUILDING_FOR_ALLOSPHERE=0
BUILDING_FOR_LINUX=0
CUDALEGACY=1
MAKE_VERBOSE=1
RUN_TARGET=0
RUN_CMAKE=0
DEPLOY_TO_ALLOSHARE=0
DEBUG_MODE=0

for i
  do
    case $i in
    -a | --allosphere)
      BUILDING_FOR_ALLOSPHERE=1
    ;;
    -l | --linux)
      BUILDING_FOR_LINUX=1
    ;;
    -d | --deploy)
      DEPLOY_TO_ALLOSHARE=1
    ;;
    -q | --quiet)
      MAKE_VERBOSE=0
    ;;
    -r | --run)
      RUN_TARGET=1
    ;;
    -c | --cmake)
      RUN_CMAKE=1
    ;;
    -v | --valgrind)
      DEBUG_MODE=1
    ;;
    esac
done



mkdir build
cd build
rm bin/${TARGET}

if [ $RUN_CMAKE = 1 ]; then
  #echo "RUNNING CMAKE"
  cmake .. -DDIR=${DIRECTORY} -DSRC=${FILENAME} -DBUILDING_FOR_ALLOSPHERE=$BUILDING_FOR_ALLOSPHERE -DBUILDING_FOR_LINUX=$BUILDING_FOR_LINUX -DCUDALEGACY=$CUDALEGACY -DDEBUGMODE=$DEBUG_MODE
fi

make VERBOSE=$MAKE_VERBOSE CPPFLAGS=-DCTL_MAKER  
if [ $DEPLOY_TO_ALLOSHARE = 1 ]; then
  cp bin/$TARGET /alloshare/pablo/
fi

if [ $RUN_TARGET = 1 ]; then
  ./bin/$TARGET
fi

