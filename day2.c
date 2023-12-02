#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>

#define INPUT_FILE "day2.txt"

int main(void)
{
    const char filename[] = INPUT_FILE;
    FILE* input = fopen(filename, "r");
    if (!input)
    {
        fprintf(stderr, "Cant load file %s.\n", filename);
        exit(EXIT_FAILURE);
    }

    int game = 0;
    int sum = 0;
    int sum_of_powers = 0;
    bool good_game = true;

    char* line = NULL;
    size_t len = 0;
    ssize_t read = 0;
   
    while ((read = getline(&line, &len, input)) != -1)
    {
        game++;
        good_game = true;

        int min_red = 0;
        int min_green = 0;
        int min_blue = 0;
        int power = 0;

        int blue = 0, green = 0, red = 0;

        char* pos = strchr(line, ':');
        pos += 2;

        char* sets[20];
        int i = 0;
        char* set = strtok(pos, ";");

        do {
            printf("draft: %s\n", set);
            sets[i++] = set;
            set = strtok(NULL, ";");
        }
        while (set != NULL);  // split by drafts
        
        for (int n = 0; n < i; ++n)
        {
            char* cube = strtok(sets[n], " ,");
            // printf("token: %s\n", cube);
            do {
                int value = atoi(cube);

                cube = strtok(NULL, " ,");
                if (cube[0] == 'b') blue = value;          
                if (cube[0] == 'r') red = value;          
                if (cube[0] == 'g') green = value;          

                if (red > 12 || 
                        green > 13 ||
                        blue > 14) good_game = false;

                if (min_red < red) min_red = red;
                if (min_green < green) min_green = green;
                if (min_blue < blue) min_blue = blue;

                cube = strtok(NULL, " ,");

            } while(cube != NULL);
        }
        
        // printf("red: %d, green: %d, blue: %d\n", red, green, blue);
        if (good_game) sum += game;
        power = min_red * min_green * min_blue;
        sum_of_powers += power;
    }
    fclose(input);

    printf("Sum of IDs: %d\n", sum);
    printf("Sum of powers: %d\n", sum_of_powers);

    return 0;
}

