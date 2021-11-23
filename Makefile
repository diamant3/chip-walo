CC     = gcc
CFLAGS = -Os

SDL_WINDOWS = -lmingw32 -lSDL2main -lSDL2 
SDL_LINUX   = -lm -lSDL2

LIBS    = -L  ~\scoop\apps\sdl2\2.0.16\lib
INCLUDE = -I  ~\scoop\apps\sdl2\2.0.16\include

SRCS = chip-walo.c system.c peripherals.c
OUT  = chip-walo

BUILD = $(CC) $(SRCS) -o bin/$(OUT) $(CFLAGS)

$(shell mkdir -p bin)

ifeq ($(OS), Windows_NT)
	BUILD += $(SDL_WINDOWS) $(LIBS) $(INCLUDE)
else
	BUILD += $(SDL_LINUX)
endif

$(OUT):$(SRCS)
	$(BUILD)
