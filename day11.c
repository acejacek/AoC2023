#include <stdio.h>
#include <stdlib.h>

#define INPUT_FILE "day11.txt"

// change for part 2
#define PART 1

typedef struct
{
    long int x;
    long int y;
} Galaxy;

long int distance (const Galaxy* a, const Galaxy* b)
{
    long int dx = labs(a->x - b->x);
    long int dy = labs(a->y - b->y);

    return dx + dy;
}

int main(void)
{
    Galaxy* galaxies = NULL;
    int count = 0;

    const char filename[] = INPUT_FILE;
    FILE* input = fopen(filename, "r");
    if (!input)
    {
        fprintf(stderr, "Can't load file %s.\n", filename);
        exit(EXIT_FAILURE);
    }

    char* line = NULL;
    size_t len = 0;
    ssize_t read = 0;

#if PART == 1
    size_t expanse = 1;
#else
    size_t expanse = 1000000 - 1;
#endif

    int y = 0;
    long int width = 0;
    while ((read = getline(&line, &len, input)) != -1)
    {
        int galaxy_found = 0;
        for (int i = 0; i < read; ++i)
        {
            if (line[i] == '#')
            {
                galaxy_found = 1;
                galaxies = (Galaxy*) realloc(galaxies, (count + 1) * sizeof(Galaxy));
                if (! galaxies)
                {
                    fprintf(stderr, "Can't allocate memory");
                    fclose(input);
                    exit(EXIT_FAILURE);
                }
                
                galaxies[count].x = i;
                galaxies[count].y = y;
                count++;
                if (width < i) width = i;
            }
        }
        if (! galaxy_found) y += expanse;  // expand Y
        y++;
    }
    fclose(input);
    width++;

    // expand X;
    for (long int x = 0; x < width; ++x)
    {
        int galaxy_found = 0;
        for (int i = 0; i < count; ++i)
            if (galaxies[i].x == x)
            {
                galaxy_found = 1;
                break;
            }

        if (! galaxy_found)
        {
            for (int i = 0; i < count; ++i)
                if (galaxies[i].x > x)
                    galaxies[i].x += expanse;
            width += expanse;
            x += expanse;
        }
    }

    long int sum = 0;
    for (int i = 0; i < count - 1; ++i)
        for (int j = i + 1; j < count; ++j)
            sum += distance(galaxies + i, galaxies + j);

    printf("Sum of distances: %ld\n", sum);

    free(galaxies);
    return 0;
}

