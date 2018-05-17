#! /bin/sh

# This script can be placed anywhere on the rover computer, provided the repository has
# been setup in the correct location

killall research_rover

cd ~

if [ ! -d rover_research ]; then
	git clone http://github.com/doublejinitials/rover_research
fi

cd rover_research

git fetch --all
git reset --hard origin/master

cd ~

if [ ! -d build_rover_research ]; then
	mkdir build_rover_research
fi

cd build_rover_research

qmake ~/rover_research/rover_research_rover.pro -r -spec linux-g++
make

echo ""
echo "Finished."


