# WolveSlayer

WolveSlayer is a 3D hack'n'slash game for the Nintendo DS released in 2007. It
got the 1st place of the [NEO Spring Coding Compo 2007](https://web.archive.org/web/20210208021659/https://www.neoflash.com/forum/index.php?topic=3895.0)
in the category of GBA/NDS Game.

The [original WolveSlayer](https://web.archive.org/web/20161225102440/http://payk.drunkencoders.com/)
was built for the toolchain and libraries available in 2007. Everything has
changed a lot since then (especially libnds). This repository is a port of the
game to BlocksDS, which has required heavy modifications of the original code.

The main difference for players of the game is that it is no longer needed to
copy the `wolveslayer` folder to the root of the SD card. It is enough to copy
the `.nds` file and load it with a loader that supports the `argv` protocol and
DLDI patching.

I've cleaned up the source code of this game a lot so that it can be useful as
reference for other developers. I've also fixed a few bugs that were present in
the original source, and restored some features that were disabled in the
original release of the game, you can read about them in the "Unused content"
section below.

![Title screen](screenshots/screenshot-1.png) ![House](screenshots/screenshot-2.png)
![Village](screenshots/screenshot-3.png) ![Forest](screenshots/screenshot-4.png)
![Entrance](screenshots/screenshot-5.png) ![Temple](screenshots/screenshot-6.png)

## Guide

The objective of this game is to find and slay the boss wolf. You will find
several enemies along the way.

- D-Pad: Move character.
- A: Open doors (only one door in the game!).
- B: Attack (in areas with enemies).

The original developer recorded a video of himself beating the game, you can
watch it [here](https://www.youtube.com/watch?v=q-SZ4lWB1Qs).

## Build instructions

Install [BlocksDS](https://blocksds.github.io/docs/).

Run `make` in the root of this repository.

## Unused content

The original source code had several features that didn't make it to the
released version of the game (most likely due to lack of time to create content
to make use of them). You can see a demo map in the branch `debug` of this
repository, but it is very crude.

- The engine supports a pause menu with an inventory management system. Item
  attributes are loaded from a text file present in the filesystem, and they can
  be assigned an icon that will be displayed in the item selection menu. The
  actual images for the menu are missing, and no item description or icons were
  present in the original release of the game/

- The engine has an NPC system. It is possible to create villagers in a specific
  location, assign them a direction, and they can move (and collide with things
  and the player!). They can also have dialog associated to them. Unfortunately,
  no NPCs were present in the original release of the game. You can use the
  model of the player as an NPC, for example.

- It is possible to display messages in the touch screen (but the text frame
  image wasn't present in the original release). They can be shown right after
  loading a map, when talking to NPCs, and there was some code to show the
  message when interacting with a door (in case the player couldn't open it, for
  example).

- All game maps are marked as `inside`, none of them are marked as `outside`.
  Maps with the `outside` attribute have a day/night light cycle (which seems to
  be unfinished), and they are a lot brighter than `inside` maps.

- The engine supports ground water textures, but none of them were ever used in
  the original maps.

## Credits

Developed by [Firehazard Studio](http://www.firehazard-studio.com)

- Code: Christian Hoffmann ([Payk](http://www.payk.drunkencoders.com))
- Textures & Objects: Christian Moor (Garel)
- Menus GFX: Abel Santos (Alfatreze)
- Editor: Marcus Schebesta
- Supervision: Chris Strauss (Fire-WSP)
- Characters: Simon Klein (Pnyx)
- Music: Jeffrey Felt (DimensionT)

Thanks to:

- Dovoto & WinterMute: For libnds.
- Chishm & GPF: For filesystems.
- Liranuna: For his modplayer sourcecode.
- Webez, Gabebear & Dovoto: For many 3D hints.
- Dovoto: For hosting my page too.
- Alfatreze: For menus, graphics and webpage layout.

Port to BlocksDS by AntonioND.
