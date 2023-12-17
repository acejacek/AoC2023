#include <stdio.h>
#include <stdlib.h>

#define INPUT_FILE "day16.txt"

typedef struct
{
    char place;
    unsigned char energized;
} Grid;

typedef enum { NORTH = 1, EAST = 2, SOUTH = 4, WEST = 8 } Dir;

void run_light(Grid* grid, int x, int y, int h, int w, Dir dir)
{
    if (x < 0 || x == w || y < 0 || y == h) return;

    int i = y * w + x;
    if (grid[i].energized & dir) return; // the light already run in this direction;
    
    grid[i].energized |= dir;

    switch(grid[i].place)
    {
        case '.':
            if (dir == NORTH) run_light(grid, x, y-1, h, w, dir);
            if (dir == SOUTH) run_light(grid, x, y+1, h, w, dir);
            if (dir == EAST) run_light(grid, x+1, y, h, w, dir);
            if (dir == WEST) run_light(grid, x-1, y, h, w, dir);
            break;

        case '\\':
            if (dir == NORTH) dir = WEST;
            else if (dir == SOUTH) dir = EAST;
            else if (dir == WEST) dir = NORTH;
            else dir = SOUTH;

            if (dir == NORTH) run_light(grid, x, y-1, h, w, dir);
            if (dir == SOUTH) run_light(grid, x, y+1, h, w, dir);
            if (dir == EAST) run_light(grid, x+1, y, h, w, dir);
            if (dir == WEST) run_light(grid, x-1, y, h, w, dir);
            break;

        case '/':
            if (dir == NORTH) dir = EAST;
            else if (dir == SOUTH) dir = WEST;
            else if (dir == WEST) dir = SOUTH;
            else dir = NORTH;

            if (dir == NORTH) run_light(grid, x, y-1, h, w, dir);
            if (dir == SOUTH) run_light(grid, x, y+1, h, w, dir);
            if (dir == EAST) run_light(grid, x+1, y, h, w, dir);
            if (dir == WEST) run_light(grid, x-1, y, h, w, dir);
            break;

        case '-':
            if (dir == NORTH || dir == SOUTH)
            {
                run_light(grid, x - 1, y, h, w, WEST);
                run_light(grid, x + 1, y, h, w, EAST);
            }
            else if (dir == EAST) run_light(grid, x + 1, y, h, w, dir);
            else if (dir == WEST) run_light(grid, x - 1, y, h, w, dir);

            break;
        case '|':
            if (dir == EAST || dir == WEST)
            {
                run_light(grid, x, y - 1, h, w, NORTH);
                run_light(grid, x, y + 1, h, w, SOUTH);
            }
            else if (dir == NORTH) run_light(grid, x, y - 1, h, w, dir);
            else if (dir == SOUTH) run_light(grid, x, y + 1, h, w, dir);
            break;
    }
}

size_t count_energized(Grid* grid, int h, int w)
{
    size_t energized = 0;
    for (int y = 0; y < h; ++y)
        for (int x = 0; x < w; ++x)
        {
            if (grid[y * w + x].energized != 0) energized++;
            grid[y * w + x].energized = 0; // reset for next run
        }
    return energized;
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
    ssize_t read = 0;

    Grid* grid = NULL;
    int h = 0;
    int w;
    while ((read = getline(&line, &len, input)) != -1)
    {
        w = read - 1;  // truncate '\n'
        grid = realloc(grid, (h + 1) * w * sizeof(Grid));
        if (! grid)
        {
            fprintf(stderr, "Can't allocate memory.\n");
            fclose(input);
            exit(EXIT_FAILURE);
        }
        
        for (int i = 0; i < w; ++i)
        {
            grid[h * w + i].place = line[i];
            grid[h * w + i].energized = 0;
        }
        h++;
    }
    fclose(input);

    // Part 1 - single run
    run_light(grid, 0, 0, h, w, EAST);
    size_t energized = count_energized(grid, h, w);
    printf("Energized cells:  %zu\n", energized);

    // Part 2 - run from all edges and find best
    size_t best_energized = 0; 
    for (int x = 0; x < w; ++x)
    {
        run_light(grid,x, 0, h, w, SOUTH);
        energized = count_energized(grid, h, w);
        if (best_energized < energized) best_energized = energized;

        run_light(grid, x, h - 1, h, w, NORTH);
        energized = count_energized(grid, h, w);
        if (best_energized < energized) best_energized = energized;
    }
    for (int y = 0; y < h; ++y)
    {
        run_light(grid, 0, y, h, w, EAST);
        energized = count_energized(grid, h, w);
        if (best_energized < energized) best_energized = energized;

        run_light(grid, w - 1, y, h, w, WEST);
        energized = count_energized(grid, h, w);
        if (best_energized < energized) best_energized = energized;
    }

    printf("Best configurtion:  %zu\n", best_energized);

    free(grid);
    return 0;
}

