#!/bin/bash

## ALLOLAUNCHER CONTENT SCRIPT feed in name of demo and -n or -r for sim or render

#export DISPLAY=:0

FOLDER=0

for i
  do
    case $i in
    -r)
      FOLDER=/alloshare/pablo/demos/
    ;;
    -n)
      FOLDER=/Users/sphere/code/pablo/horosphere/current
    ;;
    -l)
      FOLDER=/Users/wolftype/code/horosphere/build/bin/
    ;;    
    esac
done

cd $FOLDER

./$1 $2
