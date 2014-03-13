!#bin/bash

parallel-ssh -h hosts.txt "pkill $1"
