CC     = gcc
CFLAGS = -O3

SDL_WINDOWS = -lmingw32 -lSDL2main -lSDL2
SDL_LINUX   = -lm -lSDL2

LIB     = -L C:\Scoop\apps\sdl2\2.0.22\lib
INCLUDE = -I C:\Scoop\apps\sdl2\2.0.22\include

SRCS = ${wildcard src/*.c}
OUT  = chip-walo

BUILD = $(CC) $(CFLAGS) $(SRCS)

ifeq ($(OS), Windows_NT)
	BUILD += $(SDL_WINDOWS) $(LIB) $(INCLUDE) -o bin\$(OUT)
else
	BUILD += $(SDL_LINUX) -o bin/$(OUT)
endif

$(OUT):$(SRC)
	$(BUILD)