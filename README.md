To build the Assembler and Emulator:
Run:
make clean then make

This will then put the assemble and emulate executable in the build folder.

To allow debug print statements:
Set DEBUG to 1 in debug.h under src/

To build Tetris: 
You may have to use CLion (or maybe run cmake in the extension subfolder)
We've found that cmake is the most reliable way of building and running tetris 
across most machines.

For marking purposes:
Dominic and Sahil built the base game of tetris. Ahmad added some basic quality of life features to tetris. 
And Andrei helped flesh out the graphics and added lots of additional features to the game.
We just squashed commits when merging (for the purposes of keeping a cleaner code repository - it still preserves the commit info)

Though this makes it so that some people's changes aren't as visible on gitlab (ie Andrei contributed a lot)

Terairk is Ahmad Jamsari (He just forgot to change the name when using git locally hence the changes you see)