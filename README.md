# The RawSalmonEngine ![Salmon Logo](/icons/RawSalmonLogo_Ver2_128px.png)
[![Build Status](https://travis-ci.org/VectorWolf/RawSalmonEngine.svg?branch=master)](https://travis-ci.org/VectorWolf/RawSalmonEngine)

A game engine utilising *"[Tiled](http://www.mapeditor.org)" map files*.

The current state of  the project is fairly universal and resembles an **advanced map renderer for tmx files**. The clue of this engine is that it reads all game information **from the map files** an therefore the majority of game building will happen **inside the UI** of the streamlined *[Tiled Mapeditor](http://www.mapeditor.org)*. The ultimate goal of this project is to implement a full fledged game development environment via *[Tiled](http://www.mapeditor.org)*. The currently used **[Tiled](http://www.mapeditor.org) Version** is **1.1.4**
## Features ![Salmon Jump](/icons/RawSalmonLogo_Ver1_128px.png)
* Supported map orientations <img align="right" src="icons/lossyput.gif">
  - [x] Orthogonal
  - [ ] Isometric
* Supported map formats
  - [ ] CSV
  - [x] base64
  - [ ] base64(zlib)
* Supported draw order
  - [x] right-down
  - [ ] right-up
  - [ ] left-down
  - [ ] left-up
* Adaptive offscreen tile culling *(means: Better perfomance with huge maps or many Layers)*
* Static and dynamic animation support
* Layer offset and tileset offset support
* Oversized tiles which can reach from borders into the screen
* Tilesets can have *Alpha*, *Add* or *Color Modulate* Blend Mode
* Image layer suppport
* Collision detection via the *[Tiled](http://www.mapeditor.org)* hitboxes
* An event pipeline to interact with the actors
* *It actually works!*
## Dependencies
* **[SDL](http://www.libsdl.org/)** **2.0.0**+
## Compile and install
For Linux run:

<img align="right" src="/icons/TouchIt.png">

    $ mkdir build
    $ cd build
    $ cmake ..
    $ make
    
## Usage
* ./Salmon
* Loads /data/example.tmx
* Move the camera and the player via arrow keys and wasd.
## Contact
* [Agouti Games (EN)](http://game.moonlighthiker.de/index.php/en/)
* [Agouti Games (DE)](http://game.moonlighthiker.de/index.php/de/)
* [Discord Server](https://discord.gg/thAaD9e)
* [Tumblr](https://agoutigames.tumblr.com/)
* [Soundcloud](https://soundcloud.com/agoutigames)
* [Twitter](https://twitter.com/agoutigames)
## Why Salmon?
<img align="left" src="/icons/lantern_cat.png">

The main reason for developing the engine is and was the development of our own game [**Numotul - Path of Perception**](http://game.moonlighthiker.de/index.php/en/) , an action RPG in a *low fantasy* world, inspired by the culture and history of the native **Mesoamerican** tribes. The game emphasizes **narration**, **exploration** and **immersion**. You will make tough decisions enabling a truly individual gaming experience. The game's main protagonists are tribal **anthropomorphic big cats** with their own **high culture**. This is also the reason for the name of the engine, since there's not much a cat loves more than fish!
