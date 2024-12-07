# Default Compiler and Flags
CC = gcc
CFLAGS = -Wall -O2 -Wno-address -w
LDFLAGS = 

# Platform Selection
PLATFORM ?= $(shell uname)

ifeq ($(PLATFORM), Linux)
    GUI_LDFLAGS = $(shell sdl2-config --libs) -lSDL2_image -lSDL2_ttf
    GUI_CFLAGS = $(shell sdl2-config --cflags)
    SERVER_OUTPUT = server
    GUI_OUTPUT = gui
else ifeq ($(PLATFORM), windows)
    CC = x86_64-w64-mingw32-gcc
    SERVER_OUTPUT = server.exe
    GUI_OUTPUT = gui.exe
    GUI_CFLAGS = -I$(PWD)/SDL2-2.30.9/x86_64-w64-mingw32/include/SDL2
    GUI_LDFLAGS = -L/home/pedro/projects/python/chessAI/ServerAndGUI/SDL2-2.30.9/x86_64-w64-mingw32/lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lws2_32
    SERVER_LDFLAGS = -lws2_32
else
    $(error Unsupported platform: $(PLATFORM))
endif

# Source Files
SERVER_SRC = servidor.c
GUI_SRC = GUI.c stack.c communicationLayer.c

# Build Targets
all: $(SERVER_OUTPUT) $(GUI_OUTPUT)

$(SERVER_OUTPUT): $(SERVER_SRC)
	$(CC) $(CFLAGS) -o $@ $< $(SERVER_LDFLAGS)

$(GUI_OUTPUT): $(GUI_SRC)
	$(CC) $(CFLAGS) $(GUI_CFLAGS) -o $@ $^ $(GUI_LDFLAGS)

# Clean
.PHONY: clean
clean:
	rm -f $(SERVER_OUTPUT) $(GUI_OUTPUT)

