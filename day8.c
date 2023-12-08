#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INPUT_FILE "day8.txt"

typedef struct
{
    char address[4];
    char left[4];
    char right[4];
    size_t i_left;
    size_t i_right;
} Element;

typedef struct
{
    int index;
    size_t distance;
} Ghost;

// returns index of address in elements array
size_t search_element(Element* elements, const size_t count, const char* address)
{
    for (size_t i = 0; i < count; ++i)
        if (strcmp(elements[i].address, address) == 0) return i;

    fprintf(stderr, "Index not found\n");
    return 0;
}

// greatest common divisor
size_t gcd(size_t a, size_t b)
{
    while (a != b)
        if (a > b)
            a -= b;
        else
            b -= a;

    return a;
}

// least common multiple
size_t lcm(size_t a, size_t b)
{
    return a * (b / gcd(a, b));
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

    Element* elements = NULL;

    char* line = NULL;
    char instructions[300];
    size_t len = 0;

    getline(&line, &len, input);
    sscanf(line, "%s", instructions);
    int instr_len = strlen(instructions);

    int current_instr;
    size_t steps = 0;
    size_t count;

    getline(&line, &len, input);   // empty line

    // scan the file and build elements table
    for (count = 0; getline(&line, &len, input) != -1; ++count)
    {
        elements = realloc(elements, (count + 1) * sizeof(Element));
        if (! elements)
        {
            fprintf(stderr, "Can't allocate memory");
            fclose(input);
            exit(EXIT_FAILURE);
        }
        strcpy(elements[count].address, strtok(line, " =(,)\n"));
        strcpy(elements[count].left, strtok(NULL, " =(,)\n"));
        strcpy(elements[count].right, strtok(NULL, " =(,)\n"));
    }

    fclose(input);
 
    // calculate indexes to speed up walking
    for (size_t i = 0; i < count; ++i)
    {
        elements[i].i_left = search_element(elements, count, elements[i].left);
        elements[i].i_right = search_element(elements, count, elements[i].right);
    }

    // part 1
    size_t zzz_index = search_element(elements, count, "ZZZ");
    size_t index = search_element(elements, count, "AAA");

    for (current_instr = 0; index != zzz_index; ++current_instr)
    {
        if (current_instr == instr_len) current_instr = 0;

        steps++;

        if (instructions[current_instr] == 'R')
            index = elements[index].i_right;
        else
            index = elements[index].i_left;
    }
        
    printf("Part 1. Steps: %zu\n", steps);

    // part 2
    Ghost ghosts[count];
    size_t ghosts_count = 0;

    // set up starting positions for all ghosts
    for (size_t i = 0; i < count; ++i)
       if (elements[i].address[2] == 'A')
       {
            ghosts[ghosts_count].index = i;
            ghosts[ghosts_count++].distance = 0;
       }

    // record distance for ghost to nearest __Z location
    for (size_t i = 0; i < ghosts_count; ++i)
    {
        for (current_instr = 0;; ++current_instr)
        {
            if (elements[ghosts[i].index].address[2] == 'Z')
                break;

            if (current_instr == instr_len) current_instr = 0;

            ghosts[i].distance++;

            if (instructions[current_instr] == 'R')
                ghosts[i].index = elements[ghosts[i].index].i_right;
            else
                ghosts[i].index = elements[ghosts[i].index].i_left;
        }

        // brute forte would take years
        // instead, calculate steps by least common multiple
        // of all ghost's distances
        // https://en.wikipedia.org/wiki/Least_common_multiple

        // record distance as LCM with distance of previous ghost
        if (i)
            ghosts[i].distance = lcm(ghosts[i - 1].distance, ghosts[i].distance);
    }
        
    printf("Part 2. Steps: %zu\n", ghosts[ghosts_count - 1].distance);

    free(elements);
    return 0;
}

