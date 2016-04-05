#!/bin/bash

#pass in $1 executable
echo Launching $1

DIRECTORY=`dirname $1`
TARGET=`basename $1|cut -d'.' -f1 | sed -e "s|/|_|g"`
echo Directory is $DIRECTORY
echo Target is $TARGET

INTERFACE_DIRECTORY=~/code/horosphere/interfaces/
PATH_TO_INTERFACE_SERVER='~/code/interface.js/server/'

for i
  do
    case $i in
    -a | --allosphere)
      PATH_TO_INTERFACE_SERVER='~/code/interface.js/server/'
    ;;
    esac
done

#./build/bin/$TARGET &&
cd ~/code/interface.js/server/ &&
node interface.simpleserver.js --serverPort 10080 --oscOutPort 8082 --interfaceDirectory $INTERFACE_DIRECTORY
