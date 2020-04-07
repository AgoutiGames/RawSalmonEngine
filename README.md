# The RawSalmonEngine ![Salmon Logo](/icons/RawSalmonLogo_Ver2_128px.png)
[![Build Status](https://travis-ci.org/AgoutiGames/RawSalmonEngine.svg?branch=master)](https://travis-ci.org/AgoutiGames/RawSalmonEngine)

A game engine utilising *"[Tiled](http://www.mapeditor.org)" map files*.

The current state of  the project resembles a **basic programmable 2D Game Engine**. The clue of this engine is that it reads all game information **from the map files** an therefore the majority of game building will happen **inside the UI** of the streamlined *[Tiled Mapeditor](http://www.mapeditor.org)*. The ultimate goal of this project is to implement a full fledged game development environment via *[Tiled](http://www.mapeditor.org)*. The currently used **[Tiled](http://www.mapeditor.org) Version** is **1.1.4**. The minimum version is **1.0** because of the heavy use of typed tiles.
## Demo-Games
This and following games shall act as a kind of sandbox to properly showcase what the engine is actually capable of doing. Also it greatly helps with further developing the engine. Instead of theoretically figuring out what features are needed for developing a game, we experience it at first-hand. This is somewhat in the spirit of the blender open movies, but at a much smaller scale.
 
### [Calaveras](https://github.com/AgoutiGames/Calaveras)
<details><summary>Click to expand</summary>
<p></p>
 
**Calaveras** originated as an university assignment and is planned to be a smartphone "drag and release" platformer.
<p></p>
 
![Calaveras](https://github.com/AgoutiGames/Calaveras/blob/master/readme/Calaveras.gif?raw=true)

</details>

### [Plantower](https://github.com/AgoutiGames/GameOff-2019-Plantower)
<details><summary>Click to expand</summary>
<p></p>
 
**Plantower** is a tower-defense action-adventure crossover made for the Github GameOff 2019, but it wasn't finished in time.
<p></p>
 
![Plantower](https://github.com/AgoutiGames/GameOff-2019-Plantower/blob/master/readme/plantower.gif?raw=true)
 
</details>

### [Sounds of Nothingness](https://github.com/AgoutiGames/ld-45-start-with-nothing)
<details><summary>Click to expand</summary>
<p></p>
 
**Sounds of Nothingness** is a horror themed platformer made for the Ludum Dare 45.
<p></p>

![Sounds of Nothingness](https://github.com/AgoutiGames/ld-45-start-with-nothing/blob/master/readme/nothingness.gif?raw=true)

</details>

### [Terra Australis](https://github.com/AgoutiGames/TerraAustralis)
<details><summary>Click to expand</summary>
<p></p>
 
**Terra Australis** is the first game being developed with the RawSalmonEngine. It's a platformer about one of the last thylacines of Australia searching for his fellow marsupials.
<p></p>
 
![Terra Australis](https://github.com/AgoutiGames/TerraAustralis/blob/master/essentials/showcase/gameplay.gif?raw=true)

</details>

## Features <img align="right" src="/icons/RawSalmonLogo_Ver1_128px.png">
* Orthogonal, isometric and hexagonal map orientations
* CSV, base64 and base64(zlib) map formats
* All tile draw orders even for isometric or hexagonal maps
* Adaptive offscreen tile culling *(means: Better perfomance with huge maps or many Layers)*
* Tilesets support color keying, tile spacing and borders
* Tilesets and image layers can have bmp, png, jpg and tiff format
* Static and dynamic animation support via sprite sheets
* Animation via scale and rotation of actors
* Flipped, mirrored and rotated static tiles
* Layer offset and tileset offset support
* Support for tiles greater or smaller than the base tile size
* Tilesets can have *Alpha*, *Add* or *Color Modulate* Blend Mode
* Image layer suppport with optional parallax mapping
* Easy access to custom property values set inside [Tiled](http://www.mapeditor.org)
* Collision detection via the *[Tiled](http://www.mapeditor.org)* hitboxes (only Axis Aligned Bounding Boxes yet)
* Easy loading and playback of music and sound effects (flac, ogg, mp3, wav)
* Positional sound
* Text generation and rendering of TrueTypeFonts
* Input via: keyboard, mouse, gamepad and touch(by mouse emulation)
* Web build ready via emscripten (see [SalmonEngine](https://github.com/AgoutiGames/SalmonEngine))
* *It actually works!*
## Dependencies
<img align="right" src="/icons/TouchIt.png">

* **[SDL](http://www.libsdl.org/)** **2.0.0**+
* **[TinyXML2](https://github.com/leethomason/tinyxml2)** **2.2.0**+
* **[Libb64](https://sourceforge.net/projects/libb64/)**
* **[ZLIB](https://zlib.net)**
## Compile and install
* Generally you can just use the bash scripts in the [scripts folder](/scripts)
* See [COMPILATION](/COMPILATION) for details
## Usage
* All public headers are inside the include folder
* By default only a shared library is built
* Link against libSalmon.so or libSalmon.dll
## Contact
**[Patreon](https://www.patreon.com/agoutigames) | [Homepage](https://agouti.games) | [Discord](https://discord.gg/thAaD9e) | [Twitter](https://twitter.com/agoutigames) | [itch.io](https://agoutigames.itch.io/) | [Twitch](https://www.twitch.tv/agouti_games)**
## Why Salmon?
<img align="left" src="/icons/lantern_cat.png">

The main reason for developing the engine is and was the development of our own game **Numotul - Path of Perception**, an action RPG in a *low fantasy* world, inspired by the culture and history of the native **Mesoamerican** tribes. The game emphasizes **narration**, **exploration** and **immersion**. You will make tough decisions enabling a truly individual gaming experience. The game's main protagonists are tribal **anthropomorphic big cats** with their own **high culture**. This is also the reason for the name of the engine, since there's not much a cat loves more than fish!
