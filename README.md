## CPP-8
Chip-8 interpreter created with C++17 and SFML 2.5

### Compiling
Compile in an appropriate build folder made in the root of the project so that the sound effect can be picked up.

```
mkdir build
cd build
cmake ..
make
```

### Usage
`cpp8 romPath [chip48]`

chip48 option enables compatibility with Chip-48's shift instructions.

Games I have found to require chip48:
* Space Invaders

### Keybindings
```    
Chip8 Key   Keyboard
---------   ---------
1 2 3 C     1 2 3 4
4 5 6 D     q w e r
7 8 9 E     a s d f
A 0 B F     z x c v
```

### Credits
Beep effect from http://www.freesfx.co.uk

Chip-8 documentations http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
                      http://mattmik.com/files/chip8/mastering/chip8.html