CC = gcc
CFLAGS = -Wall -Werror -Wextra -Wpedantic -std=gnu99

.PHONY: all clean 

day%: day%.c
	$(CC) $(CFLAGS) $< # -o $@

clean:
	rm -f *.o
