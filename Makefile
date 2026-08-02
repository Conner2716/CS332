CC = gcc
CFLAGS = -Wall -g
EXECS = hw3

all: $(EXECS)

hw3: mckelvyc_HW03.c
	$(CC) $(CFLAGS) -o hw3 mckelvyc_HW03.c