# Tetris

Tetris game based on OpenGL which includes GLFW, GLEW.

## Installation

Use the following commands for debian based system.

```bash
cd Desktop/
git clone https://github.com/Khushiyant/Tetris.git
cd Tetris/

chmod +x requiremets.sh
./requiremets.sh
```
or

Binaries can be downloaded from [here]() for windows users

# About
## Code Structure
The game logic is implemented in `tetris.cpp`. The exact game rules and mechanics are figured from several wikis about Tetris.

Class `Piece` represents a game piece, it defines how a piece rotates and kicks off obstacles. 

Class `Board` represents the geometric state of the board. It stores which tiles are occupied, the position of the current piece and processes required motions obeying geometric constraints. Class `Tetris` operates on `Board` and defines game timings, user input processing and scoring.

Building
--------
Make sure you install `GLFW3`, `GLM` and `freetype2` correctly.  
You can compile build files in your system by installing cmakeand using following commands:
```cmake
cd path/to/Tetris-clone/
cmake -S . -B build
cmake --build build/
```

Make sure that `resources` folder is near the executable before running. 



## Contributing
Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

Please make sure to update tests as appropriate.

## License
[GNU v3](https://www.gnu.org/licenses/gpl-3.0.en.html)