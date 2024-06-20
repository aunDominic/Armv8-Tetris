To build the Assembler and Emulator:
Run:
    make clean
    make
This will then put the assemble and emulate executable in the build folder.

To allow debug print statements:
Set DEBUG to 1 in debug.h under src/

To build Tetris: 
You may have to use CLion (or maybe run cmake in the extension subfolder)
We've found that cmake is the most reliable way of building and running tetris 
across most machines.