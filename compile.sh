#!/bin/bash

echo Compiling $1 

#calls cd build 
#cmake .. -DSRC<argument> && make argument

DIRECTORY=`dirname $1`
TARGET=`basename $1|cut -d'.' -f1 | sed -e "s|/|_|g"`
FILENAME=${TARGET}.cpp
echo Directory is $DIRECTORY
echo Filename is $FILENAME
echo Target is $TARGET


mkdir build
cd build
rm bin/${TARGET}
cmake .. -DDIR=${DIRECTORY} -DSRC=${FILENAME} -DBUILDING_FOR_ALLOSPHERE=0 -DCUDALEGACY=1
make CPPFLAGS=-DCTL_MAKER  
