CC = gcc
CFLAGS = -g3 -Os
SDL_FLAGS = -lmingw32 -lSDL2main -lSDL2 -L lib -I include
SRCS = chip-walo.c system.c peripherals.c
OUT = chip-walo

$(OUT):$(SRCS)
	$(CC) $(SRCS) $(CFLAGS) $(SDL_FLAGS) -o bin\$(OUT)