# Define the compiler
CC = gcc

# Define compiler flags
CFLAGS = -Wall -g

# Define the output executable name
EXEC = interactive-notepad

# Resource compiler
WINDRES = windres

$(EXEC) : main.o resource.res
	gcc -mwindows main.o resource.res -o $@

resource.res : resource.rc resource.h
	windres $< -O coff -o $@

.PHONY: clean
