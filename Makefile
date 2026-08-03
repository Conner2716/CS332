CC = gcc
CFLAGS = -Wall
TARGET = bonus_hw
SOURCE = mckelvyc_BonusHW.c

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCE) -lpthread

clean:
	rm -f $(TARGET) average.txt
