all:
	gcc -g -I src/include -L src/lib -o main struct.c start.c food.c snake.c main.c -lmingw32 -lSDL2main -lSDL2 -lSDL2_gfx -lSDL2_ttf
