CFLAGS = -Wall -Wextra -pedantic -std=c99

all: html-parser css-parser

html-parser css-parser: %: %.c
	$(CC) $(CFLAGS) $< -o $@
