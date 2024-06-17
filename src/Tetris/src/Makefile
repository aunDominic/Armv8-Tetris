# Compiler and flags
CC = clang
CFLAGS = -g3
LDFLAGS_LINUX = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
LDFLAGS_MACOS = -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL ../lib/libraylib.a -I../include

# Source files
SRC = raylib_game.c tetriminoes.c game.c screen_ending.c screen_gameplay.c screen_logo.c screen_options.c screen_title.c
OBJ = $(SRC:.c=.o)

# Targets
all: tetris game

tetris: $(OBJ)
ifeq ($(shell uname), Linux)
	$(CC) $(CFLAGS) $(OBJ) $(LDFLAGS_LINUX) -o tetris
else
	$(CC) $(CFLAGS) $(OBJ) $(LDFLAGS_MACOS) -o tetris
endif

game: tetriminoes.o game.o
	$(CC) $(CFLAGS) tetriminoes.o game.o -o game

# Object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up
clean:
	rm -f $(OBJ) tetris game

## Compiler and flags
#CC = clang
#CFLAGS = -g3 -MMD -MP
#LDFLAGS_LINUX = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
#LDFLAGS_MACOS = -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL ../lib/libraylib.a -I../include
#
## Source files
#SRC = $(wildcard *.c)
#OBJ = $(SRC:.c=.o)
#DEPS = $(OBJ:.o=.d)
#
## Targets
#all: tetris game
#
#tetris: $(OBJ)
#ifeq ($(shell uname), Linux)
#	$(CC) $(CFLAGS) $(OBJ) $(LDFLAGS_LINUX) -o tetris
#else
#	$(CC) $(CFLAGS) $(OBJ) $(LDFLAGS_MACOS) -o tetris
#endif
#
#game: tetriminoes.o game.o
#	$(CC) $(CFLAGS) tetriminoes.o game.o -o game
#
## Object files
#%.o: %.c
#	$(CC) $(CFLAGS) -c $< -o $@
#
## Include dependency files
#-include $(DEPS)
#
## Clean up
#clean:
#	rm -f $(OBJ) $(DEPS) tetris game