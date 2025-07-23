CC = gcc
CFLAGS = -Wall -g `pkg-config --cflags sdl3`
LDFLAGS = -lcurl `pkg-config --libs sdl3`

TARGET = renderer
SRC = renderer.c

OBJS = ../parser/html-parser.o ../parser/css-parser.o ../utils/fetch.o

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET) $(SRC) $(LDFLAGS)

clean:
	rm -f $(TARGET)
