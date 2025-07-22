CC = gcc
CFLAGS = -Wall -g `pkg-config --cflags sdl3`
LDFLAGS = `pkg-config --libs sdl3`

TARGET = renderer
SRC = renderer.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

clean:
	rm -f $(TARGET)
