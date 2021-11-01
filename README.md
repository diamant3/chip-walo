# CHIP-WALO

**CHIP-8 Emulator written in C**

# Intro
CHIP-8 is an interpreted programming language which was initially used in the late 1970s. It was made to allow more easily programed game for those computers. All programs written in CHIP-8 are run on a virtual machine which interprets each instruction.

Reference: [Wikipedia](https://en.wikipedia.org/wiki/CHIP-8)
Witty Note: "walo" is from the tagalog word of the number 8.

# Samples
## Space Invaders
![Space_Invaders.png](sample-images/Space_Invaders.png)

## Breakout
![Breakout.png](sample-images/Breakout.png)

# Usage
To download this repo, run this:
```bash
$ git clone https://github.com/diamant3/chip-walo.git 
``` 
After you download, run this:
```bash
$ cd chip-walo 
 ```
## Build
If you want to build the object files only, run this:

```bash
$ make
``` 

If you want the build the executable, run this:

```bash
$ make && make all
```

**Note: The executable file is located in ``` bin ``` folder.**

## Run
Two ways to run this:

### Windows
```
chip-walo PATH/OF/THE/ROM
```
**Note: The sdl2 is pre-installed, just build and run. the ``` sdl2.dll ``` file is required.**

### Linux
```
Untested
```

# Credits
https://en.wikipedia.org/wiki/CHIP-8 - For checking correct op code<br>
http://devernay.free.fr/hacks/chip8/C8TECH10.HTM8 Heavily used for the references and guide of correct op code<br>
https://multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/ - 1/2 of code is mostly from here.<br>


