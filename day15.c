#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define INPUT_FILE "day15.txt"
#define LABEL_SIZE 10
#define MAX_LENSES 100

typedef struct
{
    char label[LABEL_SIZE];
    int focal;
} Lens;

typedef struct
{
    Lens lens[MAX_LENSES];
    int count;
} Box;

int hash_function(char* s)
{
    int len = strlen(s);
    int hash = 0;
    for (int i = 0; i < len; ++i)
    {
        hash += s[i];
        hash *= 17;
        hash %= 256;
    }
    return hash;
}

void insert_lens(Box* box, char* label, int focal)
{
    int count = box->count; // how many lenses in this box
    // check, if lens with this label already exists:
    for (int i = 0; i < count; ++i)
    {
        if (strcmp(label, box->lens[i].label) == 0)
        {
            box->lens[i].focal = focal;
            return;
        }
    }
    // add new at the end
    strcpy(box->lens[count].label, label);
    box->lens[count].focal = focal;
    box->count++;
}
void remove_lens(Box* box, char* label)
{
    int count = box->count; // how many lenses in this box
    // find lense
    for (int i = 0; i < count; ++i)
        if (strcmp(label, box->lens[i].label) == 0)
        {
            box->lens[i].label[0] = '\0';
            box->lens[i].focal = 0;
            //shift remaining
            for (int j = i; j < count - 1; ++j)
            {
                strcpy(box->lens[j].label, box->lens[j + 1].label);
                box->lens[j].focal = box->lens[j + 1].focal;
            }
            box->count--;
            return;
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

    Box boxes[256];

    char* line = NULL;
    size_t len = 0;
    ssize_t read = 0;

    size_t part1hash = 0;
   
    while ((read = getline(&line, &len, input)) != -1)
    {
        char* s = strtok(line, ",\n");
        do {
            part1hash += hash_function(s);
        
            int i; 
            // create label
            char label[LABEL_SIZE] = { 0 };
            for (i = 0; isalpha(s[i]); ++i) label[i] = s[i];

            // use hash function to calculate box index
            int index = hash_function(label); 
            // execute one of the commands
            if (s[i] == '-')
                remove_lens(boxes + index, label);
            else if (s[i] == '=')
            {
                int focal = atoi(s + i + 1);
                insert_lens(boxes + index, label, focal);
            }
        }
        while ((s = strtok(NULL, ",\n")) != NULL);
    }
    fclose(input);

    int part2sum = 0;
    for (int i = 0; i < 256; ++i)
        for (int j = 0; j < boxes[i].count; ++j)
            part2sum += (i + 1) * (j + 1) * boxes[i].lens[j].focal;

    printf("Sum of all hash values: %zu \n", part1hash);
    printf("Focusing power: %d \n", part2sum);
    return 0;
}

