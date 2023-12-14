#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INPUT_FILE "day14.test"

// change for part 2
#define PART 2

#define PATTERN 30

typedef struct
{
    char* rocks;
    int width;
    int height;
    int pattern[PATTERN];
    int pattern_mu;
    int pattern_lam;
} Rocks;

int shift_stone_north(Rocks* r, int i)
{
    if (r->rocks[i] != 'O') return 0; // I can move only rounded rocks

    int j = i - r->width; //index of new position
    
    if (r->rocks[j] == '.')
    {
        r->rocks[j] = 'O';
        r->rocks[i] = '.';
        return 1;
    }

    return 0;
}

int shift_stone(Rocks* r, int i, int k)
{
    if (r->rocks[i] != 'O') return 0; // I can move only rounded rocks

    int j = i + k; //index of new position
    
    if (r->rocks[j] == '.')
    {
        r->rocks[j] = 'O';
        r->rocks[i] = '.';
        return 1;
    }

    return 0;
}

int weight(Rocks*r)
{
    int sum = 0;
    int dist = 0;
    for (int i = r->width * r->height; i > 0; --i)
    {
        if (i % r->width == 0) dist++;
        if (r->rocks[i-1] == 'O') sum += dist;
    }

    return sum;
}

int tilt(Rocks* r)
{
    int sum;
    do {
        sum = 0;
        for (int i = r->width; i < r->width * r->height; ++i)
            sum += shift_stone_north(r, i);
    }
    while (sum != 0);
}

void print_rocks(Rocks* r)
{
    for (int i = 0; i < r->height * r->width; ++i)
    { 
        if (i % r->width == 0) putchar('\n');
        putchar(r->rocks[i]);
    }
    putchar('\n');
}

void cycle(Rocks* r)
{
    int sum;
    do {  // north
        sum = 0;
        for (int i = r->width; i < r->width * r->height; ++i)
        {
            sum += shift_stone(r, i, -r->width);
        }
    } while (sum != 0);

    do { // west
        sum = 0;
        for (int x = 1; x < r->width; ++x)
            for (int y = 0; y < r->width * (r->height - 1); ++y)
            {
                sum += shift_stone(r, y * r->width + x, -1);
            }
    } while (sum != 0);

    do { // south
        sum = 0;
        for (int i = 0; i < r->width * (r->height - 1); ++i)
        {
            sum += shift_stone(r, i, r->width);
        }
    } while (sum != 0);

    do { // east
        sum = 0;
        for (int x = 0; x < r->width - 1; ++x)
            for (int y = 0; y < r->width * (r->height - 1); ++y)
                sum += shift_stone(r, y * r->width + x, 1);

    } while (sum != 0);
}

int check_pattern(Rocks* r)
{
    int tortoise = 10;
    int hare = tortoise + 1;
    while (r->pattern[tortoise] != r->pattern[hare])
    {
        tortoise++;
        hare += 2;
        if (hare >= PATTERN)
        {
            printf("Cant find start\n");
            return 0;
        }
    }
    r->pattern_mu = 0;
    tortoise = 0;
    while (r->pattern[tortoise] != r->pattern[hare])
    {
        tortoise++;
        hare++;
        r->pattern_mu++;

        if (hare >= PATTERN)
        {
            printf("Cant find start\n");
            return 0;
        }
    }
    r->pattern_lam = 1;
    hare = tortoise + 1;
    while (r->pattern[tortoise] != r->pattern[hare])
    {
        r->pattern_lam++;
        hare++;
        if (hare >= PATTERN)
        {
            printf("Cant find start\n");
            return 0;
        }
    }
    return 1;
}

int main(void)
{
    Rocks r;
    r.rocks = NULL;
    for (int i = 0; i < PATTERN; ++i)
        r.pattern[i] = 0;

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

    for (r.height = 0; (read = getline(&line, &len, input)) != -1; ++r.height)
    {
        r.width = read - 1;
        r.rocks = (char*) realloc(r.rocks, (r.height + 1) * r.width * sizeof(char));
        if (! r.rocks)
        {
            fprintf(stderr, "Can't allocate memory");
            fclose(input);
            exit(EXIT_FAILURE);
        }
        
        memcpy(r.rocks + (r.height * r.width), line, r.width);
    }
    fclose(input);

#if PART == 1
//    tilt(&r);
    int sum = weight(&r);
    printf("Sumarized weight: %d\n", sum);
#else

    for (size_t i = 0; i < 100; ++i)
    {
        cycle(&r);
        r.pattern[i] = weight(&r);
    }

    if (check_pattern(&r))
    {
        printf("Found repetition %d, %d\n ",r.pattern_mu, r.pattern_lam);
        size_t offset = (10000 - r.pattern_mu) % r.pattern_lam;
        printf("Result: %d\n", r.pattern[offset]);
    }
#endif

    free(r.rocks);
    return 0;
}

