#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

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
    bool good_game;

    char* line = NULL;
    size_t len = 0;
    ssize_t read = 0;
   
    while ((read = getline(&line, &len, input)) != -1)
    {
        game++;
        good_game = true;

        int red = 0, green = 0, blue = 0;
        int min_red = 0, min_green = 0, min_blue = 0;

        char* pos = strchr(line, ':') + 2; // skip Game 1:

        char* cube = strtok(pos, " ,;");
        do {
            int value = atoi(cube);  // first token is with value

            cube = strtok(NULL, " ,"); // second is with color
            if (cube[0] == 'r') red = value;          
            if (cube[0] == 'g') green = value;          
            if (cube[0] == 'b') blue = value;          

            if (red > 12 || 
                    green > 13 ||
                    blue > 14) good_game = false;

            if (min_red < red) min_red = red;
            if (min_green < green) min_green = green;
            if (min_blue < blue) min_blue = blue;

            cube = strtok(NULL, " ,"); // get next token
        }
        while(cube);

        if (good_game) sum += game;
        sum_of_powers += min_red * min_green * min_blue;
    }

    fclose(input);

    printf("Sum of IDs: %d\n", sum);
    printf("Sum of powers: %d\n", sum_of_powers);

    return 0;
}

