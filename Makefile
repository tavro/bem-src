CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99
LDFLAGS = -lcurl

OBJS = parser/render-tree.o parser/html-parser.o parser/css-parser.o utils/fetch.o

render-tree: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o render-tree $(LDFLAGS)

parser/%.o: parser/%.c
	$(CC) $(CFLAGS) -c $< -o $@

utils/%.o: utils/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f parser/*.o utils/*.o render-tree
