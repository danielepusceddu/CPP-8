## CPP-8
Chip-8 interpreter created with C++17.

### Compiling
CPP-8 can be compiled to use SFML 2.5 or SDL2 as multimedia libraries.

SDL2 is the default. Give a "-DCPP8_ENGINE=SFML" to cmake to compile for SFML.

Compile in an appropriate build folder made in the root of the project so that the sound effect can be picked up.

```
mkdir build
cd build
cmake .. #Uses SDL2
#cmake .. -DCPP8_ENGINE=SFML  #Uncomment this line to use SFML instead.
make
```

### Usage
`cpp8 romPath [chip48] [-s <outputScale>]`

`-s <outputScale>` multiplies the original resolution (64x32) by outputScale.

outputScale should not be less than 1. The default is 10.

chip48 option enables compatibility with Chip-48's shift instructions.

Games I have found to require chip48:
* **Space Invaders.** Hit detection seems to break without it.

### Keybindings
```    
Chip8 Key   Keyboard
---------   ---------
1 2 3 C     1 2 3 4
4 5 6 D     q w e r
7 8 9 E     a s d f
A 0 B F     z x c v
```

ESC to quit the interpreter.

Pause or F1 to pause the interpreter.

### Credits
Beep effect from http://www.freesfx.co.uk

Chip-8 documentations used:
* http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
* http://mattmik.com/files/chip8/mastering/chip8.html