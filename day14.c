#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INPUT_FILE "day14.txt"

// change for part 2
#define PART 2

#define PATTERN 500

typedef struct
{
    char* rocks;
    int width;
    int height;
    int pattern[PATTERN];
    int pattern_mu;
    int pattern_lam;
} Rocks;

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

void tilt(Rocks* r)
{
    int sum;
    do {
        sum = 0;
        for (int i = r->width; i < r->width * r->height; ++i)
            sum += shift_stone(r, i, -r->width);
    }
    while (sum != 0);
}

void cycle(Rocks* r)
{
//    static int c = 0; if (c++ == 4) exit(1);
    int sum;
    do {  // north
        sum = 0;
        for (int i = r->width; i < r->width * r->height; ++i)
            sum += shift_stone(r, i, -r->width);
    } while (sum != 0);

    do { // west
        sum = 0;
        for (int x = 1; x < r->width; ++x)
            for (int y = 0; y < r->height; ++y)
                sum += shift_stone(r, y * r->width + x, (-1));
    } while (sum != 0);

    do { // south
        sum = 0;
        for (int i = 0; i < r->width * (r->height - 1); ++i)
            sum += shift_stone(r, i, r->width);
    } while (sum != 0);

    do { // east
        sum = 0;
        for (int x = 0; x < r->width - 1; ++x)
            for (int y = 0; y < r->height; ++y)
                sum += shift_stone(r, y * r->width + x, 1);
    } while (sum != 0);
}

/*
 * bruteforce would take ages
 * instead, find repetability in results and calculate:
 * mu - from when repetition stars
 * lam - what's the repetition period is
 * I expect results will look like this:
 *
 *    ##
 *   #  #  #  #  #  #
 *  #    ## ## ## ## ##.....
 * #    ^__^
 *      | \
 *     mu  lam
 */
int check_pattern(Rocks* r)
{
    int inter[100] = { 0 };
    int ind = 0;
    int sign = 1;
    int old_sign = sign;
    // built table with intervals between peaks
    
    for (int i = 0; i < PATTERN; ++i)
    {
        int delta = r->pattern[i + 1] - r->pattern[i];
        if (delta > 0) sign = 1;
        else sign = -1;

        if (old_sign != sign)
        {
            inter[ind++] = i;
            old_sign = sign;
        }
    }

    // now find arbitrary 10 equal distances
    for (int i = 0; i < ind - 10; ++i)
    {
        int is_equal = 1;
        int delta = inter[i + 2] - inter[i];
        for (int j = 1; j < 10; ++j)
        {
            if (inter[i + j + 2] - inter[i + j] != delta)
            {
                is_equal = 0;
                break;
            }
        }
        if (is_equal)
        {
            r->pattern_mu = inter[i];
            r->pattern_lam = inter[i + 2] - inter[i];
            return 1;
        }
    }
    // not found
    return 0;
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

    tilt(&r);
    int sum = weight(&r);
    printf("Sumarized weight: %d\n", sum);

#else

    for (size_t i = 0; i < PATTERN; ++i)
    {
        cycle(&r);
        r.pattern[i] = weight(&r);
    }

    if (check_pattern(&r))
    {
//        printf("Found repetition %d, %d\n",r.pattern_mu, r.pattern_lam);
        size_t offset = ((1000000000l - r.pattern_mu) % r.pattern_lam) + r.pattern_mu;
        printf("Result: %d\n", r.pattern[offset - 1]);
    }
    else printf("Can't find soultion. Change parameters.\n"); 
#endif

    free(r.rocks);
    return 0;
}

