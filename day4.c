#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define INPUT_FILE "day4.txt"

#define MAX_LINE_NUMBERS 300
#define MAX_WINNING_NUMBERS 15

int main(void)
{
    const char filename[] = INPUT_FILE;
    FILE* input = fopen(filename, "r");
    if (!input)
    {
        fprintf(stderr, "Cant load file %s.\n", filename);
        exit(EXIT_FAILURE);
    }

    size_t sum = 0;
    int copies[MAX_LINE_NUMBERS] = { 0 };

    char* line = NULL;
    size_t len = 0;
    ssize_t read = 0;
   
    while ((read = getline(&line, &len, input)) != -1)
    {
        int winning[MAX_WINNING_NUMBERS] = { 0 }; // array with winning numbers
        int winning_n = 0;

        char* pos = line + 5;   // skip "Card"
        int card = atoi(pos);   // read card number
        copies[card] += 1;      // increase copes counter by original card

        pos = strchr(line, ':') + 2;

        for (; *pos != '|'; ++pos)         // build array with winning numbers
        {
            if (! isdigit(*pos)) continue; // skip spaces, I need my pos right
                                          
            int num = atoi(pos);
            if (num > 9) pos++;

            winning[winning_n++] = num;
        }

        size_t points = 0;
        int copy = 0;
        for (; *pos != '\n'; ++pos) // scan all remaining and match against winning
        {
            if (! isdigit(*pos)) continue;

            int num = atoi(pos);
            if (num > 9) pos++;

            for (int i = 0; i < winning_n; ++i)
                if (num == winning[i])   // scanned number is among winning
                {
                    if (points == 0) points = 1;  // first point
                    else points <<= 1;            // double points

                    copy++;   // which of the next cards needs to be "copied"?
                    copies[card + copy] += copies[card];  // increase copy counter
                                                          // of next card
                }
        }
        sum += points;
    }
    fclose(input);

    printf("Sum of all points: %ld\n", sum);

    sum = 0;
    for (int i = 0; i < MAX_LINE_NUMBERS; ++i)
        sum += (size_t) copies[i];

    printf("Total number of scratchcards: %ld\n", sum);

    return 0;
}

