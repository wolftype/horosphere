#!/bin/bash

#pass in $1 executable
echo Launching $1

DIRECTORY=`dirname $1`
TARGET=`basename $1|cut -d'.' -f1 | sed -e "s|/|_|g"`
echo Directory is $DIRECTORY
echo Target is $TARGET

INTERFACE_DIRECTORY=~/code/pablo/horosphere/interfaces/
PATH_TO_INTERFACE_SERVER='~/code/interface.js/server/'

for i
  do
    case $i in
    -a | --allosphere)
      PATH_TO_INTERFACE_SERVER='~/code/interface.js/server/'
    ;;
    esac
done



cd ~/code/pablo/interface.js/server/ &&
node interface.simpleserver.js --serverPort 1066 --oscOutPort 8082 --interfaceDirectory $INTERFACE_DIRECTORY &
cd ~/code/pablo/horosphere/ && ./build/bin/$TARGET $2
echo run '/alloshare/pablo/xKnot3 -r'
