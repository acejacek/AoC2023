#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INPUT_FILE "day13.txt"

typedef struct
{
    char* data;
    int height;
    int width;
} Pattern;

int search_v_line(Pattern* p, int old_line)
{
    for (int x = 0; x < p->width - 1; ++x)
    {
        if (x + 1 == old_line) continue; // for part 2, ignore old mirror lines

        int a = 0;
        int b = 1;
        int is_mirror = x + 1;  // the split line
        do {
            for (int y = 0; y < p->height; ++y)
            {
                int i_a = (y * p->width) + x - a;
                int i_b = (y * p->width) + x + b;
                // check if all elements in lines a and are equal
                if (p->data[i_a] != p->data[i_b])
                {
                    is_mirror = 0;
                    break;
                }
            }
            if (is_mirror)
            {
                // extend search
                a++;
                b++;
                // if behind the edge, seach is completed
                if (x - a < 0 || x + b == p->width) return is_mirror;
            }
        } while (is_mirror);
    }
    return 0;
}

// same as above, just traversimg y/x
int search_h_line(Pattern* p, int old_line)
{
    for (int y = 0; y < p->height - 1; ++y)
    {
        if (y + 1 == old_line) continue;

        int a = 0;
        int b = 1;
        int is_mirror = y + 1;
        do {
            for (int x = 0; x < p->width; ++x)
            {
                int i_a = (y - a) * p->width + x;
                int i_b = (y + b) * p->width + x;
                if (p->data[i_a] != p->data[i_b])
                {
                    is_mirror = 0;
                    break;
                }
            }
            if (is_mirror)
            {
                a++;
                b++;
                if (y - a < 0 || y + b == p->height) return is_mirror;
            }
        } while (is_mirror);
    }
    return 0;
}

int fix_smudge(Pattern* p)
{
    int orig_v = search_v_line(p, 0);
    int orig_h = search_h_line(p, 0);

    // brute-force all elements, and check for different mirror results
    for (int i = 0; i < p->width * p->height; ++i)
    {
        if (p->data[i] == '#') p->data[i] = '.';
        else p->data[i] = '#';

        // ignore orig mirror lines
        int new_v = search_v_line(p, orig_v);
        int new_h = search_h_line(p, orig_h);

        int score = new_v + new_h * 100;

        if (score != 0) return score;

        if (p->data[i] == '#') p->data[i] = '.';
        else p->data[i] = '#';
    }
    return 0;
}

void build_pattern(Pattern* p, char* line, int read, size_t* score1, size_t* score2)
{
    if (read < 2) // new pattern is ready
    {
        *score1 += search_v_line(p, 0);
        *score1 += search_h_line(p, 0) * 100;

        *score2 += fix_smudge(p);

        free(p->data);
        p->data = NULL;
        p->height = 0;
    }
    else  // continue loading pattern
    {
        p->width = read - 1 ;
        p->data = realloc(p->data, (p->height + 1) * p->width * sizeof(char));
        if (! p->data)
        {
            fprintf(stderr, "Can't allocate memory");
            exit(EXIT_FAILURE);
        }

        memcpy(p->data + (p->height * p->width), line, p->width);
        p->height++;
    }
}

int main(void)
{
    const char filename[] = INPUT_FILE;
    FILE* input = fopen(filename, "r");
    if (!input)
    {
        fprintf(stderr, "Can't load file %s.\n", filename);
        exit(EXIT_FAILURE);
    }

    Pattern pattern = { NULL, 0, 0 };

    char* line = NULL;
    size_t len = 0;
    ssize_t read = 0;
    size_t score1 = 0;
    size_t score2 = 0;
   
    while ((read = getline(&line, &len, input)) != -1)
        build_pattern(&pattern, line, read, &score1, &score2);

    // call very last time after reading whole file, to finalize last pattern
    build_pattern(&pattern, line, read, &score1, &score2);

    printf("Result part 1: %zu\n", score1); 
    printf("Result part 2: %zu\n", score2); 
    
    fclose(input);
    free(pattern.data);
    return 0;
}

