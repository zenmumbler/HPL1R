HPL1 Rehatched
==============
A project to create a modernised version of Frictional Games' HPL1 game engine and Penumbra game series source code.

Building
--------
All dependencies are included in the repository. This is mostly because many of them are (very) old versions.

### macOS

* Open the Xcode project
* Select Penumbra Overture target
* Build the target
* Copy the built application and the `rehatched` folder into your pre-existing Penumbra game folder
* Run the app (macOS 10.13.6 High Sierra or newer required)

I have it setup to build the app straight into the game folder and I've symlinked the rehatched folder
in there as well, but setting that up is left as an exercise for the reader.

Project Goals
-------------
The original goal was to get Penumbra Overture compiling as a 64-bit Mac app so that it
can be enjoyed on macOS 10.15 and newer that have removed 32-bit Intel support. This has been achieved.

There are still some outstanding physics and game playability and reliability issues.
These will be resolved as the project progresses. Upcoming big ticket items include:

* Modernise graphics backend to core OpenGL profile (3.3+)
* Windows and Linux builds
* Gamepad support

Project Links
-------------
This repo integrates 3 repos from Frictional Games:

* [HPL1Engine](https://github.com/FrictionalGames/HPL1Engine)
* [OALWrapper](https://github.com/FrictionalGames/OALWrapper)
* [PenumbraOverture](https://github.com/FrictionalGames/PenumbraOverture)

There are a couple of reasons why this is not just a fork of one or more of those.
The 3 projects were designed to share some dependencies and are closely interrelated
so having them in one repo with the dependencies makes development simpler. In addition,
some of the dependencies are very old and/or have been specially built and/or modified
to work with this project. Finally, Frictional Games is a small studio and do not have
time to properly respond to pull requests etc so their original repos should be considered
more of a preservation effort than a live project.

Licence and Copyright
---------------------
Penumbra, Penumbra: Overture and HPL are all Copyright Frictional Games. 
All code and assets included in this repo are licensed under GPL3, see the LICENSE file for details.

The Penumbra games and all their assets are not included with this project,
you need a pre-existing installation of the game.
