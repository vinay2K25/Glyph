CC = cc
CFLAGS = -Wall -Wextra -pedantic -std=c23
LIBS = -lncurses
SRC = Source/*.c
TARGET = glyph

$(TARGET):
	$(CC) $(SRC) $(LIBS) -o $(TARGET) $(CFLAGS)

clean:
	rm -f $(TARGET)