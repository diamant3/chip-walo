<p align="center"><img src="assets/chip-walo.gif" width="640px"></p>

# CHIP-WALO

![C](https://img.shields.io/badge/Code-C-green?style=for-the-badge)
![SDL2](https://img.shields.io/badge/SDL2-blue?style=for-the-badge)

> CHIP-8 is an interpreted programming language which was initially used in the late 1970s. It was made to allow more easily programed game for those computers. All programs written in CHIP-8 are run on a virtual machine which interprets each instruction.<br>
Reference: [Wikipedia](https://en.wikipedia.org/wiki/CHIP-8)

*"WALO" is from the tagalog word of number 8.*

## Features

<ul>
  <li>Working sound system</li>
  <li>Cross-platform(Windows/Linux)</li>
</ul>


## Game samples

### Space Invaders

<img src="assets/Space_Invaders.png" width="480px">

### Breakout

<img src="assets/Breakout.png" width="480px">

## Building from source

### Linux

Pre-requisites:
* git
* gcc
* make
* libsdl2-2.0
* libsdl2-dev

Download the repo:

```bash 

git clone https://github.com/diamant3/chip-walo.git
``` 

After you download, run this:

```bash

cd chip-walo 
```
and this: 

```bash
make
```

### Windows

Pre-requisites:
* gcc
* make
* scoop

**Install scoop first**, refer to this guide to install [scoop](https://scoop.sh/).

After you install the scoop, run this:

```bash

scoop bucket add extras && scoop install sdl2
```

Download the [chip-walo](https://github.com/diamant3/chip-walo/archive/refs/heads/main.zip).

Extract the zip file then run this inside the folder:

```bash

make
```

Link: [SDL2 Runtime Binaries](https://www.libsdl.org/download-2.0.php)
Note: ```SDL2.dll is``` **required**. Put it inside the ``` bin ``` folder.

## Usage

<table>
  <tr>
      <td>1</td>
      <td>2</td>
      <td>3</td>
      <td>4</td>
  </tr>
  <tr>
      <td>Q</td>
      <td>W</td>
      <td>E</td>
      <td>R</td>
  </tr>
  <tr>
      <td>A</td>
      <td>S</td>
      <td>D</td>
      <td>F</td>
  </tr>
  <tr>
      <td>Z</td>
      <td>X</td>
      <td>C</td>
      <td>V</td>
  </tr>
</table>

> This is the key mappings of the chip-walo, use keyboard to control.

### Run

```bash
chip-walo PATH/OF/THE/ROM
```

*The executable file is located in ``` bin ``` folder.*

## Credits

[SDL2](https://www.libsdl.org/) - graphics API that i used to make this.<br>
[scoop](https://scoop.sh/) - This is i used to fix the sdl2 dependencies.<br>
[Cowgod](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM) - Heavily used for the references of op codes.<br>
[Wikipedia](https://en.wikipedia.org/wiki/CHIP-8) - Another reference for op codes.<br>
[Multigesture](https://multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/) - the emulation of graphics of chip-8 is mostly from here.<br>
[Tobiasvl](https://tobiasvl.github.io/blog/write-a-chip-8-emulator/)- Clear explanation about chip-8 development.<br>
[Chip-8](https://chip-8.github.io/links/) - Chip-8 Resources.<br>
[Zophar.net](https://www.zophar.net/pdroms/chip8.html) - chip-8 ROMS.<br>

## Contribution

This is an educational project and feel free to contribute!

*Please read the [Contributing](https://github.com/diamant3/chip-walo/blob/main/CONTRIBUTING.md).*

*Please read the [Code of Conduct](https://github.com/diamant3/chip-walo/blob/main/CODE_OF_CONDUCT.md).*
