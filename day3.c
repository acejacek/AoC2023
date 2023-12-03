#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define INPUT_FILE "day3.txt"

typedef struct
{
    int count;
    int ratio;
} Gears;

/* sets number_good if symbol is around a digit
 * if '*' symbol found, add info to table of Gears 
 * function has way too many parameters */
void check_symbol(const char* data, const size_t i, const size_t len, const int width, int* number_good, Gears* gears, const int a)
{
    for (int x = (int)i - 1; x <= (int)i + 1; ++x)
        for (int y = x - width; y <= x + width; y += width)
            if (y >= 0 && y < (int)len)
                if (data[y] != '.' && 
                        data[y] != '\n' && 
                        isdigit(data[y]) == 0)
                {
                    if (*number_good == 0 && data[y] == '*')
                    {
                        if (gears[y].count == 0)
                            gears[y].ratio = a;
                        else
                            gears[y].ratio *= a;

                        gears[y].count++;
                    }

                    *number_good = 1;
                    return;
                    /* [FIXME] there is a potential bug, when other symbol will be found prior to '*'
                     * Then, further searching is ignored and gear ratio wrongly calculated
                     * It worked OK for my dataset, however. */
                }
}

/* 1. load text file into continuous block of memory
 * 2. calculate table width etc.
 * 3. scan all searching for numbers
 * 4. scan surroundings of every digit searching symbols
 * 5. calculate results */

int main(void)
{
    const char filename[] = INPUT_FILE;
    FILE* input = fopen(filename, "r");
    if (!input)
    {
        fprintf(stderr, "Can't find file %s.\n", filename);
        exit(EXIT_FAILURE);
    }

    if (fseek(input, 0L, SEEK_END) != 0)
    {
        fprintf(stderr, "Can't seek to end of file %s.\n", filename);
        fclose(input);
        exit(EXIT_FAILURE);
    }

    long bufsize = ftell(input);
    if (bufsize == -1)
    {
        fprintf(stderr, "Can't get size of file %s.\n", filename);
        fclose(input);
        exit(EXIT_FAILURE);
    }

    char* data = malloc(sizeof(char) * (bufsize + 1));
    if (!data)
    {
        fprintf(stderr, "Can't allocate %ld bytes of data.\n", bufsize + 1);
        fclose(input);
        exit(EXIT_FAILURE);
    }

    // this is expensive; probably I should do that better
    Gears* gears = calloc(sizeof(Gears), bufsize);
    if (!gears)
    {
        fprintf(stderr, "Can't allocate %ld bytes of data.\n", sizeof(Gears) * bufsize);
        fclose(input);
        free(data);
        exit(EXIT_FAILURE);
    }

    if (fseek(input, 0L, SEEK_SET) != 0)
    {
        fprintf(stderr, "Can't seek to beginning of file %s.\n", filename);
        fclose(input);
        free(data);
        free(gears);
        exit(EXIT_FAILURE);
    }

    size_t len = fread(data, sizeof(char), bufsize, input);
    if (ferror(input) != 0)
    {
        fprintf(stderr, "Error reading file %s.\n", filename);
        fclose(input);
        free(data);
        free(gears);
        exit(EXIT_FAILURE);
    }

    fclose(input);

    char* eol = memchr(data, '\n', len);
    int width = 1 + eol - data;
    
    int sum = 0;

    for (size_t i = 0; i < len; ++i)
    {
        if (! isdigit(data[i])) continue;

        int a = atoi(data + i);
        int number_good = 0;

        check_symbol(data, i, len, width, &number_good, gears, a);

        if (a > 9)
            check_symbol(data, ++i, len, width, &number_good, gears, a);

        if (a > 99)
            check_symbol(data, ++i, len, width, &number_good, gears, a);

        if (number_good) sum += a;
    }

    printf("Sum of part numbers: %d\n", sum);

    sum = 0;
    for (int i = 0; i < bufsize; ++i)
        if (gears[i].count == 2) sum += gears[i].ratio;

    printf("Sum of gear ratios: %d\n", sum);

    free(data);
    free(gears);
    return 0;
}

