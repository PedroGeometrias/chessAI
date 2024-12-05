# Compiler and Flags
CC = gcc
CFLAGS = -Wall -O2 -Wno-address
LDFLAGS = 

# Detect platform
OS := $(shell uname)
ifeq ($(OS), Linux)
    PLATFORM = linux
    GUI_LDFLAGS = $(shell sdl2-config --libs) -lSDL2_image -lSDL2_ttf
    GUI_CFLAGS = $(shell sdl2-config --cflags)
    SERVER_OUTPUT = server
    GUI_OUTPUT = gui
else ifeq ($(OS), Windows_NT)
    PLATFORM = windows
    CC = x86_64-w64-mingw32-gcc
    SERVER_OUTPUT = server.exe
    GUI_OUTPUT = gui.exe
    GUI_LDFLAGS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf
else
    $(error Unsupported platform: $(OS))
endif

# Source Files
SERVER_SRC = servidor.c
GUI_SRC = GUI.c stack.c communicationLayer.c

# Default Target
all: $(SERVER_OUTPUT) $(GUI_OUTPUT)

# Compile Server
$(SERVER_OUTPUT): $(SERVER_SRC)
	$(CC) $(CFLAGS) -o $@ $<

# Compile GUI
$(GUI_OUTPUT): $(GUI_SRC)
	$(CC) $(CFLAGS) $(GUI_CFLAGS) -o $@ $^ $(GUI_LDFLAGS)

# Clean Target
.PHONY: clean
clean:
	rm -f $(SERVER_OUTPUT) $(GUI_OUTPUT)
