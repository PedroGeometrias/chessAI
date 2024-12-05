# Chess Ai with an server and gui 

I have written a chess AI with TensorFlow and wanted to integrate it with a GUI written in C, so I made a server in 
C that handles the communication. I have drawn a diagram for this project. It's in Portuguese, but it should help with 
understanding my idea:
![Diagrama](https://github.com/PedroGeometrias/chessAI/diagrama) 

## PYTHON side of things

- aiSetup.py: Contains the AI creation and training logic. I developed it in Google Colab, but I left it here as a backup.

- convertPgnCsv.py: Converts all .pgn files in a directory to .csv, saving only the 
  information that I thought was important for AI training.

- intermediateTranslation.py: Represents the IT on the diagram. It handles communication in real time with the server 
  and also helps with other logic.

- All other .py files: These are used to save data from each game played. They create .html files with that 
  information, nicely compiled into graphs.

## Server
- Simple server that I have written in c, sending strings from one application to another

## GUI

- GUI.c: The main logic for the board, using SDL2.
- communicationLayer.c: Handles communication between the GUI and the server.
- stack.c: Implements a stack data structure for undo-redo features.

# HOW TO RUN

 The explanation for windows maybe wrong, but I will leave the binaries on releases anyway

## What you will need

- python3
- SDL2
- SDL2_image
- SDL2_ttf
- gcc
- make (I have written a make file to help the compiling process)
- mingw32
- mingw32-make

## Linux
 you can run make on the ServerAndGUI dir, it will generate two targets, server and gui, or you can use this command on the dir with the files
```
gcc GUI.c -o gui communicationLayer.c stack.c -lSDL2 -lSDL2_image -lSDL2_ttf -lm
gcc servidor.c -o server

```
 and for the python lybraries you can install them from the requirements.txt that I have left on the AI dir, you can 
 do something like this:
```
pip install -r requirements.txt

```
 > **Note:** remember, when running, the server comes first, then the gui, then the intermediateTranslation.py

## Windows
 you can run make for both if you have mingw32-make

```
mingw32-make server
mingw32-make gui

```
 you can install the python lybraries in the same way as linux I think

```
pip install -r requirements.txt

```

 > **Note:** remember, when running, the server comes first, then the gui, then the intermediateTranslation.py

### Contact
 
 email : harop6770@gmail.com
 discord username : PedroGeometrias



