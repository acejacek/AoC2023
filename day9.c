#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INPUT_FILE "day9.txt"

int only_zeros(int* h, int len)
{
    for (int i = 0; i < len; ++i)
        if (h[i] != 0) return 0;

    return 1;
}

int* alloc_mem(int len)
{
    int* mem = malloc(sizeof(int) * len);
    if (! mem)
    {
        fprintf(stderr, "Can't allocate memory");
        exit(EXIT_FAILURE);
    }

    return mem;
}

int extrapolate_history(int* h, int len)
{
    if (only_zeros(h, len)) return 0;

    int* extrapolate = alloc_mem(len - 1);

    for (int i = 0; i < len - 1; ++i)
        extrapolate[i] = h[i + 1] - h[i]; // diff

    int res = extrapolate_history(extrapolate, len - 1);

    free(extrapolate);

    return h[len - 1] + res;
}

int extrapolate_past_history(int* h, int len)
{
    if (only_zeros(h, len)) return 0;

    int* extrapolate = alloc_mem(len - 1);

    for (int i = len - 1; i > 0; --i)
        extrapolate[i - 1] = h[i] - h[i - 1]; // diff

    int res = extrapolate_past_history(extrapolate, len - 1);

    free(extrapolate);

    return h[0] - res;
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

    char* line = NULL;
    size_t len = 0;
    
    int sum_future = 0;
    int sum_past = 0;

    while (getline(&line, &len, input) != -1)
    {
        int* values = NULL;
        int v;

        char* token = strtok(line, " \n");
        for (v = 0; token != NULL; ++v)
        {
            values = (int*) realloc(values, (v + 1) * sizeof(int));
            if (! values)
            {
                fprintf(stderr, "Can't allocate memory");
                fclose(input);
                exit(EXIT_FAILURE);
            }
            values[v] = atoi(token);

            token = strtok(NULL, " \n");
        }

        sum_future += extrapolate_history(values, v);
        sum_past += extrapolate_past_history(values, v);

        free(values);
    }
    fclose(input);

    printf("Part 1: %d \n", sum_future);
    printf("Part 2: %d \n", sum_past);

    return 0;
}

