#!/bin/bash

#pass in name of executable to run and optional argument to send to bossanova
if [ $# == 2 ]; then
  scp build/bin/$1 create@bossanova:/tmp/
  ssh create@bossanova && ./tmp/$1
else
  echo Deploying $1 To All Computers\' /tmp folder Listed in hosts.txt
  parallel-scp -h hosts.txt build/bin/$1 /tmp/
  echo Launching $1 To All Said Computers
  parallel-ssh -h hosts.txt "export DISPLAY=:0; /tmp/$1"
fi
