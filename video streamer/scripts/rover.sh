#! /bin/sh

# This script can be placed anywhere on the rover computer, provided the repository has
# been setup in the correct location

LD_LIBRARY_PATH=/home/soro/build_rover_research/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}
export LD_LIBRARY_PATH
exec "/home/soro/build_rover_research/bin/rover" ${1+"$@"}
