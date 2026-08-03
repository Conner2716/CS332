CC = gcc
CFLAGS = -Wall
TARGET = bonus_hw
SOURCE = mckelvyc_Bonus_HW.c

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCE) -lpthread

clean:
	rm -f $(TARGET) average.txt