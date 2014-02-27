
echo Deploying $1 To All Computers in Listed hosts.txt

parallel-scp -h hosts.txt build/demos/$1 /tmp/
