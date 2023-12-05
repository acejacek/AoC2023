#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

#define INPUT_FILE "day5.txt"

#define PART 2

typedef struct
{
    int step;
    size_t dest;
    size_t source;
    size_t range;
} Mapping;

typedef struct
{
    size_t value;
#if PART == 2
    size_t range;
#endif
} Seed;

int map_number(size_t* number, Mapping* map)
{
    if (*number < map->source ||
        *number > map->source + map->range - 1) return 0;   // not in range

    *number -= map->source + map->dest;
    return 1;
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

    Seed* seeds = NULL;
    size_t seed_count = 0;

    Mapping* mapping = NULL;
    int mapping_count = 0;

    char* line = NULL;
    size_t len = 0;
    ssize_t read = 0;

    getline(&line, &len, input);    // read header with initial seeds

    char* pos = line + 7;  // skip "seeds: "
    char* num_str = strtok(pos, " ");

    while (num_str != NULL)
    {
        seeds = realloc(seeds, (seed_count + 1) * sizeof(Seed));
        seeds[seed_count].value = atol(num_str);

#if PART == 2
        num_str = strtok(NULL, " ");
        seeds[seed_count].range = atol(num_str);
#endif
        seed_count++;
        num_str = strtok(NULL, " ");
    }


    int step = 0;
    while ((read = getline(&line, &len, input)) != -1)
    {
        if (islower(*line))
        {
            step++;
            printf("%s", line);
            continue;
        }

        if (! isdigit(*line)) continue;

        mapping = realloc(mapping, (mapping_count + 1) * sizeof(Mapping));
        sscanf(line, "%zu %zu %zu",
                &mapping[mapping_count].dest,
                &mapping[mapping_count].source,
                &mapping[mapping_count].range
                );

        mapping[mapping_count++].step = step;
    }
    fclose(input);

#if PART == 1
    for (size_t s = 0; s < seed_count; ++s)
        for (int st = 0; st <= step; ++st)
        {
            int unchanged = 1;

            for (int m = 0; m < mapping_count && unchanged; ++m)
            {
                if (mapping[m].step != st) continue;
                if (map_number(&seeds[s].value, mapping + m))
                {
                    unchanged = 0;
                    break;
                }
            }
        }

    size_t lowest = SIZE_MAX;
    for (size_t i = 0; i < seed_count; ++i)
        if (lowest > seeds[i].value) lowest = seeds[i].value;

#else

    size_t lowest = SIZE_MAX;
    for (size_t s = 0; s < seed_count; ++s)
    {
        size_t* seeds_range = malloc(seeds[s].range * sizeof(size_t));

        for (size_t ts = 0; ts < seeds[s].range; ++ts)
        {
            seeds_range[ts] = seeds[s].value + ts;

            for (int st = 0; st <= step; ++st)
            {
                int unchanged = 1;

                for (int m = 0; m < mapping_count && unchanged; ++m)
                {
                    if (mapping[m].step != st) continue;
                    if (map_number(seeds_range + ts, mapping + m))
                    {
                        unchanged = 0;
                        break;
                    }
                }
            }
        }
        for (size_t i = 0; i < seeds[s].range; ++i)
                if (lowest > seeds_range[i]) lowest = seeds_range[i];

        free(seeds_range);
    }

#endif

    printf("Lowest location number is: %zu\n", lowest);

    free(seeds); 
    return 0;
}
