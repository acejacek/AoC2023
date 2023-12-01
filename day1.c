#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define INPUT_FILE "day1.txt"

// change for part 2:
#define PART 1

int is_digit(const char* x)
{
    const char c = x[0];
    if (isdigit(c)) return c - '0';

#if PART == 2
    if (strncmp("zero", x, 4) == 0) return 0;
    if (strncmp("one", x, 3) == 0) return 1;
    if (strncmp("two", x, 3) == 0) return 2;
    if (strncmp("three", x, 5) == 0) return 3;
    if (strncmp("four", x, 4) == 0) return 4;
    if (strncmp("five", x, 4) == 0) return 5;
    if (strncmp("six", x, 3) == 0) return 6;
    if (strncmp("seven", x, 5) == 0) return 7;
    if (strncmp("eight", x, 5) == 0) return 8;
    if (strncmp("nine", x, 4) == 0) return 9;
#endif

    return -1;
}

int main(void)
{
    const char filename[] = INPUT_FILE;
    FILE* input = fopen(filename, "r");
    if (!input)
    {
        fprintf(stderr, "Cant load file %s.\n", filename);
        exit(EXIT_FAILURE);
    }

    int sum = 0;

    char* line = NULL;
    size_t len = 0;
    ssize_t read = 0;
   
    while ((read = getline(&line, &len, input)) != -1)
    {
        char text[6];
        int x = 0, res = -1;

        for (int i = 0; res == -1; ++i)
        {
            strncpy(text, line + i, 5);
            res = is_digit(text);
        }

        x = res * 10;
        res = -1;

        for (int i = read; res == -1; --i)
        {
            strncpy(text, line + i, 5);
            res = is_digit(text);
        }

        x += res;
        sum += x;
    }

    fclose(input);

    printf("Sum of all calibration values is: %d\n", sum);
    return 0;
}

