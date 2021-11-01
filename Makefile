CC = gcc
CFLAGS = -g -Wall -Wextra -Wpedantic

SDL_FLAGS = -lmingw32 -lSDL2main -lSDL2 -L lib -I include

OBJS = chip-walo.o system.o peripherals.o
SRCS = chip-walo.c system.c peripherals.c

OUT = chip-walo

chip-walo.o: $(SRCS)
	$(CC) $(CFLAGS) $(SDL_FLAGS) -c $(SRCS) 

all:
	$(CC) $(CFLAGS) $(OBJS) $(SDL_FLAGS) -o bin/$(OUT)

clean:
	rm -f $(OBJS) bin/$(OUT)