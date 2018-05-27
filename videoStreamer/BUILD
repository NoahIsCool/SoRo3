Ok so I kinda fucked things up a little bit. If you have never put this code on the machine
before then skip this. This doesnt apply to you. However, if you are trying to build this
after a version before, there are some steps you need to do to ensure that it will build
correctly.

Qt was supposed to delete all traces of the socket class when I got rid of it from the 
videoServer class and the videoDisplayer class. But it didnt. So if you see socket.anything
in videoServer or videoDisplayer, delete them, run make clean,  make and that should fix
one problem.

Now, Qt was being a bitch about linking the Networking project so now you need to make sure
that you remove all traces of the old Networking or everything will crash when the sockets
connect. Just run these commands:

sudo rm /usr/lib/libNetworking.so
sudo rm /usr/lib/libNetworking.so.1
sudo rm /usr/lib/libNetworking.so.1.0
sudo rm /usr/lib/libNetworking.so.1.0.0

Dont ask me why it installed 4 copies of the same damn thing, I dont know. I think its a 
waste of harddrive space but whatever, they are all symbolic links anyways.

BUILD INSTRUCTIONS:
Rover build instructions:
1) clone the repo:
git clone 
2) build Networking
cd videoStreamer/Networking
qmake Networking.pro -r -spec linux-g++
make
make install
3) build videoStreamer
cd ../videoStreamer
make
make install

Mission Control build instructions:
1)clone the repo:
git clone
2) build Networking
cd videoStreamer/Networking
qmake Networking.pro -r -spec linux-g++
make
make install
3) build videoDisplayer
if you are using qt creator then dont worry about this, just run it from the creator
cd ../videoDisplayer
make
make install
