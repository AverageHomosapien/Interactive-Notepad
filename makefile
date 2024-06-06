# Define the compiler
CC = gcc

# Define compiler flags
CFLAGS = -Wall -g

# Define the output executable name
EXEC = interactive-notepad

# Resource compiler
WINDRES = windres

# Collect all .c files in the current directory
SRCS = $(wildcard *.c)

# Generate a list of .o files from the .c files
OBJS = $(SRCS:.c=.o)

# Default target to build the executable
$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJS)

# Pattern rule to compile .c files to .o files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Compile the resource script to an object file
resource.o: resource.rc
	$(WINDRES) $< -o $@

# Link object files to create the executable
$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $@

# Clean up generated files
.PHONY: clean
clean:
	rm -f $(EXEC) $(OBJS)
