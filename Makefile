CC = gcc
CFLAGS = -g -Wall -Wextra -Wpedantic
SDL_FLAGS = -lmingw32 -lSDL2main -lSDL2 -L lib -I include

SRCS = chip-walo.c system.c peripherals.c
OUT = chip-walo

all:
	$(CC) $(CFLAGS) $(SDL_FLAGS) -o bin/$(OUT)

clean:
	rm -f bin/$(OUT)