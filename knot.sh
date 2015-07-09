#1/bin/bash

./current/xKnotAudio &
./current/xKnotGLV &
cd ~/code/interface.js/server/ &&
node interface.simpleserver.js --serverPort 10080 --oscOutPort 8082 --interfaceDirectory ~/Documents/interfaces

