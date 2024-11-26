CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -std=gnu99 -ggdb

.PHONY: all clean 

day%: day%.c
	$(CC) $(CFLAGS) $< # -o $@

run:
	./a.out

clean:
	rm -f *.o
