
#!/bin/bash

echo Compiling $1 for ALLOSPHERE

#calls cd build 
#cmake .. -DSRC<argument> && make argument


DIRECTORY=`echo $1 |cut -d'/' -f1 -f2`
FILENAME=`echo $1 |cut -d'/' -f2 |cut -d'.' -f1 | sed -e "s|/|_|g"`
TARGET=${FILENAME}
echo Directory is $DIRECTORY
echo Filename is $FILENAME
echo Target is $TARGET

mkdir build
cd build
rm bin/${TARGET}
cmake .. -DDIR=${DIRECTORY} -DSRC=../../$1 -DALLOSPHERE=1
make
