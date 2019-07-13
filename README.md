## CPP-8
Chip-8 interpreter created with C++17.

It can use SDL2, SFML2 or WebAssembly to run on the web.

### Compiling for desktop
CPP-8 can be compiled to use SFML 2.5 or SDL2 as multimedia libraries.

SDL2 is the default. Give "-DCPP8_ENGINE=SFML" to cmake to compile for SFML.

Compile in an appropriate build folder made in the root of the project.

The executable can then be moved anywhere you want and it will still work. The sound effect is now embedded into the executable.

```
mkdir build
cd build
cmake .. #Uses SDL2
#cmake .. -DCPP8_ENGINE=SFML  #Uncomment this line to use SFML instead.
make
```

### Compiling for web with WebAssembly
CPP-8 can also be compiled with emscripten.

Inside of your build folder, create a "c8games" folder with all of the Chip-8 games you want to play. Then:

```
emcc ../src/Chip8.cpp ../src/Chip8_SDL.cpp ../src/main.cpp -std=c++17 -O3 --preload-file c8games/ -s USE_SDL=2
cp ../src/page.html a.html
```

After this, edit the `games` and `c48games` variables in `a.html` to include your games. Run the files on a server.
To try it locally, use `python2 -m SimpleHTTPServer 8080` in your build directory and then go to localhost:8080/a.html

### Command Line Arguments
`cpp8 romPath [chip48] [-s <outputScale>]`

`-s <outputScale>` multiplies the original resolution (64x32) by outputScale.

outputScale should not be less than 1. The default is 10.

chip48 option enables compatibility with Chip-48's shift instructions.

Games I have found to require chip48:
* **Space Invaders.** Hit detection seems to break without it.
* **Tic Tac Toe.** Or the game won't recognize when a player wins.

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
