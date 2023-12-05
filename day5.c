#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <pthread.h>

#define INPUT_FILE "day5.txt"

#define PART 1

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

#if PART == 2
typedef struct
{
    Seed seed;
    Mapping* mappings;
    int mapping_count;
    int step;
    size_t* lowest;

} ThreadParams;

pthread_mutex_t mutex;
#endif

int map_number(size_t* number, const Mapping* map)
{
    if (*number < map->source ||
        *number > map->source + map->range - 1) return 0;   // not in range

    *number -= map->source;
    *number += map->dest;
    return 1;
}

void map_seed(size_t* value, const int step, const Mapping* mappings, const int mapping_count)
{
    for (int st = 0; st <= step; ++st)
        for (int m = 0; m < mapping_count; ++m)
        {
            if (mappings[m].step != st) continue; // apply only mappings for actual step
            if (map_number(value, mappings + m))
                break;  // break as soon as mapping is applied, no duplicate mappings
        }
}

#if PART == 2
// multi-thread function to calculate seeds mapping
void* map_all(void* params)
{
    size_t low = SIZE_MAX;
    ThreadParams* p = (ThreadParams*) params;
    size_t a_seed;

    for (size_t ts = 0; ts < p->seed.range; ++ts)
    {
        a_seed = p->seed.value + ts; // get seed from range

        // use the same function as in Part 1
        map_seed(&a_seed, p->step, p->mappings, p->mapping_count); 

        if (low > a_seed) low = a_seed;
    }

    pthread_mutex_lock(&mutex);
    if (*p->lowest > low) *p->lowest = low;
    pthread_mutex_unlock(&mutex);

    return NULL;
}
#endif

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

    Mapping* mappings = NULL;
    int mapping_count = 0;

    char* line = NULL;
    size_t len = 0;
    ssize_t read = 0;

    getline(&line, &len, input);    // read header with initial seeds

    char* pos = line + 7;  // skip "seeds: "
    char* num_str = strtok(pos, " ");

    // create array of initial seeds
    while (num_str != NULL)
    {
        seeds = realloc(seeds, (seed_count + 1) * sizeof(Seed));
        if (!seeds)
        {
            fprintf(stderr, "Can't allocate enough memory.\n");
            fclose(input);
            exit(EXIT_FAILURE);
        }
        seeds[seed_count].value = atol(num_str);

#if PART == 2
        // for part 2 keep track on range
        num_str = strtok(NULL, " ");
        seeds[seed_count].range = atol(num_str);
#endif
        seed_count++;
        num_str = strtok(NULL, " ");
    }

    // create array with mapping data
    int step = 0;
    while ((read = getline(&line, &len, input)) != -1)
    {
        if (islower(*line)) // new mapping step
        {
            step++;
            continue;
        }

        if (! isdigit(*line)) continue; // search for line starting with digits

        mappings = realloc(mappings, (mapping_count + 1) * sizeof(Mapping));
        if (!mappings)
        {
            fprintf(stderr, "Can't allocate enough memory.\n");
            fclose(input);
            free(seeds);
            exit(EXIT_FAILURE);
        }

        sscanf(line, "%zu %zu %zu",
                &mappings[mapping_count].dest,
                &mappings[mapping_count].source,
                &mappings[mapping_count].range
                );

        mappings[mapping_count++].step = step;
    }
    fclose(input);

    size_t lowest = SIZE_MAX;

#if PART == 1

    for (size_t s = 0; s < seed_count; ++s)
    {
        map_seed(&seeds[s].value, step, mappings, mapping_count); 
        // after applying all mappings, check, if this is the lowest value (location)
        if (lowest > seeds[s].value) lowest = seeds[s].value;
    }
#else

/*
 * Number of seeds to calculate in Part #2 is massive.
 * On my thin Linux machine single thread took 94 minutes to calculate
 * In multithread (on 4 core processor) it took just shy of 30 minutes.
 */
    pthread_t thread[seed_count];
    pthread_mutex_init(&mutex, NULL);
    ThreadParams thread_param[seed_count];

    // start separate thread for each initial seed and range
    for (size_t s = 0; s < seed_count; ++s)
    {
        thread_param[s].seed = seeds[s];
        thread_param[s].step = step;
        thread_param[s].mappings = mappings;
        thread_param[s].mapping_count = mapping_count;
        thread_param[s].lowest = &lowest;

        if (pthread_create(thread + s, NULL, map_all, (void*) (thread_param + s)) !=0 )
        {
            perror("Failed to create thread!");
            free(seeds);
            exit(EXIT_FAILURE);
        }
    }

    for (size_t s = 0; s < seed_count; ++s)
        if (pthread_join(thread[s], NULL) != 0)
        {
            perror("Failed to join thread!");
            free(seeds);
            exit(EXIT_FAILURE);
        }

    pthread_mutex_destroy(&mutex);

#endif

    printf("Lowest location number is: %zu\n", lowest);

    free(seeds); 
    free(mappings); 
    return 0;
}

