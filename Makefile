# Compiler
CC = gcc

# Compiler and linker flags
CFLAGS = -Wall -Wextra -std=c99 -Iinclude
LDFLAGS = -LC:/raylib/w64devkit/x86_64-w64-mingw32/lib -lraylib -lgdi32 -lwinmm

# Source files
SRC = game.c

# Default target
all: game

# Link object file to create the executable
game: $(SRC)
	$(CC) -o $@ $< $(CFLAGS) $(LDFLAGS)

.PHONY: all clean run

clean:
	rm -f game.exe

# Run the program
run: game.exe
	IF EXIST game.exe game.exe
