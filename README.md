# dcmo5

This is a version of DCMO5 with some changes for an emulation closer to the real hardware.
 * Uses the MO5 color palette, not the TO7/70 one.

Unfortunately, Daniel decided to develop DCMOTO as a Windows-only and closed source emulator, leaving users of other operating systems without update for a few years. This is an attempt to improve the situation for people not using windows and wanting to run Thomson games anyway.

# How to build

> This part requires to be used to build C/C++ programs from sources.

## Required

* A C/C++ toolchain, usually gcc, make and usual build tools on Linux.
* CMake
* The development libraries for SDL.

## Walkthrough

This procedure will create a working directory for CMake to work inside, leaving the minimal amount of CMake artifacts outside of this directory.

> The command are given for a typical Linux shell.

### Getting the source and preparing folders

Nothing out of the ordinary here. At the end of this sequence, we are into a subfolder `build`, that was created inside the working copy of the project, and will be the place where CMake generate all its artefacts.

```bash
git clone https://github.com/pulkomandy/dcmo5.git
cd dcmo5
mkdir build
cd build
```

### Use CMake to prepare and configure the build process

CMake is invoked a first time to generate the default build AND list any available option.

```bash
cmake -LH ..
```

At the time of writing, the listed options are displayed like this :
```
// Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel ...
CMAKE_BUILD_TYPE:STRING=

// Install path prefix, prepended onto install directories.
CMAKE_INSTALL_PREFIX:PATH=/usr/local

// To enable a print of the scancode on the standard output, usefull when crafting a layout.
DEBUG_SCANCODE:BOOL=OFF

// Path to a library.
SDLMAIN_LIBRARY:FILEPATH=/usr/lib/x86_64-linux-gnu/libSDLmain.a

// Path to a file.
SDL_INCLUDE_DIR:PATH=/usr/include/SDL

// Where the SDL Library can be found
SDL_LIBRARY:STRING=/usr/lib/x86_64-linux-gnu/libSDLmain.a;/usr/lib/x86_64-linux-gnu/libSDL.so;-lpthread

// To enable a layout crafted for typematrix keyboard with bepo layout.
USE_TYPEMATRIX_BEPO_LAYOUT:BOOL=OFF
```

If one wants to change an option, one invokes CMake again. E.g., to activate the option `DEBUG_SCANCODE` :

```bash
cmake -LH -DDEBUG_SCANCODE=ON ..
```

CMake list the options again, if the invocation was correct, the option is changed :
```
// Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel ...
CMAKE_BUILD_TYPE:STRING=

// Install path prefix, prepended onto install directories.
CMAKE_INSTALL_PREFIX:PATH=/usr/local

// To enable a print of the scancode on the standard output, usefull when crafting a layout.
DEBUG_SCANCODE:BOOL=ON

// Path to a library.
SDLMAIN_LIBRARY:FILEPATH=/usr/lib/x86_64-linux-gnu/libSDLmain.a

// Path to a file.
SDL_INCLUDE_DIR:PATH=/usr/include/SDL

// Where the SDL Library can be found
SDL_LIBRARY:STRING=/usr/lib/x86_64-linux-gnu/libSDLmain.a;/usr/lib/x86_64-linux-gnu/libSDL.so;-lpthread

// To enable a layout crafted for typematrix keyboard with bepo layout.
USE_TYPEMATRIX_BEPO_LAYOUT:BOOL=OFF
```

### Use cmake to perform the build

```bash
cmake --build .
```

If all went well, the program is ready :

```
Scanning dependencies of target dcmo5
[ 10%] Building C object CMakeFiles/dcmo5.dir/source/dc6809emul.c.o
[ 20%] Building C object CMakeFiles/dcmo5.dir/source/dcmo5boutons.c.o
[ 30%] Building C object CMakeFiles/dcmo5.dir/source/dcmo5devices.c.o
[ 40%] Building C object CMakeFiles/dcmo5.dir/source/dcmo5dialog.c.o
[ 50%] Building C object CMakeFiles/dcmo5.dir/source/dcmo5emulation.c.o
[ 60%] Building C object CMakeFiles/dcmo5.dir/source/dcmo5keyb.c.o
[ 70%] Building C object CMakeFiles/dcmo5.dir/source/dcmo5main.c.o
[ 80%] Building C object CMakeFiles/dcmo5.dir/source/dcmo5options.c.o
[ 90%] Building C object CMakeFiles/dcmo5.dir/source/dcmo5video.c.o
[100%] Linking C executable dcmo5
[100%] Built target dcmo5
```

### Run the project

To be able to load disk images and tape images, copy the `software` folder into the `build` :

```bash
cp -R ../software .
```

Run the program and enjoy :

```bash
./dcmo5
```
