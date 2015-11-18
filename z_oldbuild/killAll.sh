#!/bin/bash

parallel-ssh -h util/hosts.txt "pkill $1"
