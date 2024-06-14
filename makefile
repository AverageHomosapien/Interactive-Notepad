# Define the compiler
CC = gcc

# Define compiler flags
CFLAGS = -Wall -g -mwindows

# Adding linker flags
LDFLAGS = -lcomctl32

# Define the output executable name
EXEC = interactive-notepad

# Resource compiler
WINDRES = windres

all: $(EXEC)

$(EXEC) : main.o resource.res
	gcc -mwindows main.o resource.res -lcomctl32 -o $@

resource.res : resource.rc resource.h
	windres $< -O coff -o $@

clean:
	rm -f *.o d $(EXEC)
