#/bin/bash
docker pull agodio/itba-so:1.0
docker run -v ${PWD}:/root --security-opt seccomp:unconfined -ti agodio/itba-so:1.0 
