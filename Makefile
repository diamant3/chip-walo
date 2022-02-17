CC     = gcc
CFLAGS = -Os

SDL_WINDOWS = -lmingw32 -lSDL2main -lSDL2
SDL_LINUX   = -lm -lSDL2

# use $env:USERPROFILE/$USERPROFILE in powershell
PROFILE = $(shell echo %USERPROFILE%)

LIBS    = -L $(PROFILE)\scoop\apps\sdl2\2.0.20\lib
INCLUDE = -I $(PROFILE)\scoop\apps\sdl2\2.0.20\include

SRCS = src/*.c
OUT  = chip-walo

BUILD = $(CC) $(CFLAGS) $(SRCS) -o bin/$(OUT)

$(shell mkdir bin)
ifeq ($(OS), Windows_NT)
	BUILD += $(SDL_WINDOWS) $(LIBS) $(INCLUDE)
else
	BUILD += $(SDL_LINUX)
endif

$(OUT):$(SRCS)
	$(BUILD)
