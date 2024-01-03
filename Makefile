CC = gcc
CFLAGS = -Wall -Werror -Wextra -Wpedantic -std=gnu99 -ggdb

.PHONY: all clean 

day%: day%.c
	$(CC) $(CFLAGS) $< # -o $@

clean:
	rm -f *.o
