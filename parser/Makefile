CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99

OBJS = render-tree.o html-parser.o css-parser.o

render-tree: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o render-tree

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o render-tree