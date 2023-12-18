#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INPUT_FILE "day18.txt"

// change for part 2
#define PART 1

typedef struct
{
    long int x;
    long int y;
} Node;

int main(void)
{
    int exit_code = EXIT_SUCCESS;
    const char filename[] = INPUT_FILE;
    FILE* input = fopen(filename, "r");
    if (!input)
    {
        fprintf(stderr, "Can't load file %s.\n", filename);
        exit_code = EXIT_FAILURE;
        goto cleanup;
    }

    char* line = NULL;
    size_t l = 0;
   
    Node* dig = malloc(sizeof(Node));
    if (! dig)
    {
        fprintf(stderr, "Can't allocate memory");
        exit_code = EXIT_FAILURE;
        goto cleanup;
    }
    dig[0].x = 0; // init position
    dig[0].y = 0;
    
    int len = 0;
    long int area2 = 0;
    long int x = 0;
    long int y = 0;
    for (int i = 1; getline(&line, &l, input) != -1; ++i)
    {
        dig = realloc(dig, (i + 1) * sizeof(Node));
        if (! dig)
        {
            fprintf(stderr, "Can't allocate memory");
            exit_code = EXIT_FAILURE;
            goto cleanup;
        }

        char dir;
        long int steps;
        char color[7];
        sscanf(line, "%c %ld (#%6s)", &dir, &steps, color);
#if PART == 1
        switch(dir)
        {
            case 'R':
                x += steps;
                break;
            case 'L':
                x -= steps;
                break;
            case 'D':
                y += steps;
                break;
            case 'U':
                y -= steps;
        }
#else
        char hex[6] = { 0 };
        strncpy(hex, color, 5);        // extract distance
        steps = strtol(hex, NULL, 16); // convert from hex

        switch(color[5]) // direction is at last position in color
        {
            case '0':
                x += steps;
                break;
            case '2':
                x -= steps;
                break;
            case '1':
                y += steps;
                break;
            case '3':
                y -= steps;
        }
#endif
        dig[i].x = x;
        dig[i].y = y;

        // calculate 2x area from points
        area2 += dig[i - 1].x * y;
        area2 -= dig[i - 1].y * x;

        len += steps;
    }

    // https://en.wikipedia.org/wiki/Pick%27s_theorem
    long int area = len + (area2 / 2 - len / 2 + 1);
    printf("Area of the digout: %ld\n", area);

cleanup:
    if (input) fclose(input);
    free(dig);
    exit(exit_code);
}

