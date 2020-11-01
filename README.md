HPL1 Rehatched
==============

A project to create a modernised version of Frictional Games' HPL1 game engine and Penumbra game series source code.

Building
--------
Build instructions will appear here once we get it to build and run.

Goals
-----

### Current
* Create a working codebase with updated dependencies
* Deploy Penumbra: Overture on 64-bit only macOS (10.15 and later)
	* Keep backcompat to 10.13 or so

### Next (all tentative)
* Windows and Linux builds
* Add modern macOS gamepad support (for PS4 and Xbox controllers) and Xinput for Win
* Modernise graphics backend, first to recent OpenGL, then Metal/DX12/Vulkan
* Re-encode assets as glTF2, basisu textures, etc.
* Link to or integrate with upscaled asset mods
* etc...

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
Penumbra, Penumbra: Overture and HPL are all Copyright Frictional Games, see the various
licenses in the project folders. The base licence is GPL3 so this project falls under that
as well.

The non open-sourced or free assets of the Penumbra games are not included with this
project, you need a pre-existing installation of the game.
