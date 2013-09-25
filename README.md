Sigma
===

The Sigma game engine for creating games on multiple platforms. The name Sigma was chosen due to its meaning in math of summation notation. In Sigma, engine components are assigned to entities, and it is the sum of all the components that makes an entity.

Requires CMake and a compiler that supports C++11.

Prerequisites for Ubuntu 12.04:

* gcc and g++ 4.8 (http://askubuntu.com/questions/271388/how-to-install-gcc-4-8-in-ubuntu-12-04-from-the-terminal)
* Preinstalling SDL2.0: sudo apt install build-essential xorg-dev libudev-dev libts-dev libgl1-mesa-dev libglu1-mesa-dev libasound2-dev libpulse-dev libopenal-dev libogg-dev libvorbis-dev libaudiofile-dev libpng12-dev libfreetype6-dev libusb-dev libdbus-1-dev zlib1g-dev libdirectfb-dev
* SDL2.0 (http://www.libsdl.org/hg.php)
* SDL\_Image 2.0 (http://www.libsdl.org/projects/SDL_image/)
* SOIL (sudo apt-get install libsoil-dev)



Installing
===

* Run CMake and set the source directory to the root folder.
* Set the build directory to root/build/
* Configure and generate your make/project/solutions files.
* Build and run.

Visual Studio
===
You must change the startup project via right-clicking on the project in the solution explorer in order for Debugging to launch the correct program.
Also you must go into the project's properties, and under the 'Debugging' category change the 'Working Directory' to '$(OutDir)'.
