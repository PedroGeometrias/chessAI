# Definindo compilador
CC = gcc
CFLAGS = -Wall -O2 -Wno-address
LDFLAGS = 

# Fazendo isso ser multi plataforma 
OS := $(shell uname)
# caso seja linux 
ifeq ($(OS), Linux)
    PLATFORM = linux
	# bibliotecas
    GUI_LDFLAGS = $(shell sdl2-config --libs) -lSDL2_image -lSDL2_ttf
	# flags
    GUI_CFLAGS = $(shell sdl2-config --cflags)
	# outputs
    SERVER_OUTPUT = server
    GUI_OUTPUT = gui
# caso seja windows
else ifeq ($(OS), Windows_NT)
    PLATFORM = windows
	# Usar MINGW
    CC = x86_64-w64-mingw32-gcc
	# windows tem o .exe no final
    SERVER_OUTPUT = server.exe
    GUI_OUTPUT = gui.exe
	# flags
    GUI_LDFLAGS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf
else
	# caso seja alguma outra plataforma 
    $(error Unsupported platform: $(OS))
endif

# Arquivos fonte usados na compilacao
SERVER_SRC = servidor.c
GUI_SRC = GUI.c stack.c communicationLayer.c

# Resultados
all: $(SERVER_OUTPUT) $(GUI_OUTPUT)

# Server
$(SERVER_OUTPUT): $(SERVER_SRC)
	$(CC) $(CFLAGS) -o $@ $<

# GUI 
$(GUI_OUTPUT): $(GUI_SRC)
	$(CC) $(CFLAGS) $(GUI_CFLAGS) -o $@ $^ $(GUI_LDFLAGS)

# Limpo o resultado 
.PHONY: clean
clean:
	rm -f $(SERVER_OUTPUT) $(GUI_OUTPUT)
