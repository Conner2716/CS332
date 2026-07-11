# Makefile for HW 2 - search program
# Run "make" to build, "make clean" to remove the built files.

# CC is the compiler to use
CC = gcc

# CFLAGS are the flags passed to the compiler.
# -Wall turns on all common warnings (helps catch bugs)
CFLAGS = -Wall

# TARGET is the name of the final executable
TARGET = homework

# SRCS is the source file(s) to compile
SRCS = mckelvyc_HW02.c

# "make" with no arguments builds $(TARGET)
all: $(TARGET)

# Rule: to build TARGET, compile SRCS directly into it
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

# "make clean" deletes the compiled executable
clean:
	rm -f $(TARGET)

# .PHONY tells make that "all" and "clean" are not actual files,
# just labels for commands to run
.PHONY: all clean