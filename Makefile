all:
	g++ -Iinclude -Iinclude/sdl -Iinclude/headers -Llib -o find src/*.cpp -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf
