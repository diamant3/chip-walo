CC     = gcc
CFLAGS = -Os

SDL_WINDOWS = -lmingw32 -lSDL2main -lSDL2 
SDL_LINUX   = -lm -lSDL2

# use $env:USERPROFILE/$USERPROFILE in powershell
PROFILE = %USERPROFILE%

LIBS    = -L PROFILE\scoop\apps\sdl2\2.0.16\lib
INCLUDE = -I PROFILE\scoop\apps\sdl2\2.0.16\include

SRCS = chip-walo.c system.c peripherals.c
OUT  = chip-walo

BUILD = $(CC) $(SRCS) -o bin/$(OUT) $(CFLAGS)

$(shell rd /S /Q bin) 
$(shell mkdir bin)

ifeq ($(OS), Windows_NT)
	BUILD += $(SDL_WINDOWS) $(LIBS) $(INCLUDE)
else
	BUILD += $(SDL_LINUX)
endif

$(OUT):$(SRCS)
	$(BUILD)
