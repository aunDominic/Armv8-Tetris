tetris: raylib_game.c tetriminoes.c game.c screen_ending.c screen_gameplay.c screen_logo.c screen_options.c screen_title.c
	gcc -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL ../lib/libraylib.a -I../include raylib_game.c tetriminoes.c game.c screen_ending.c screen_gameplay.c screen_logo.c screen_options.c screen_title.c -o tetris  

game: tetriminoes.c game.c
	gcc tetriminoes.c game.c -o game