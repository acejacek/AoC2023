#include <stdio.h>
#include <stdlib.h>

#define INPUT_FILE "day10.txt"

typedef enum { UP, LEFT, DOWN, RIGHT } Direction;

typedef struct
{
    char shape;
    unsigned char visited;
} Pipe;

typedef struct
{
    Pipe* pipes;
    size_t width;
    size_t height;
    size_t size;
    size_t solved;
    size_t steps;
    size_t prev_x;
    size_t prev_y;
    int area;
} Maze;

// returns 1 if movement in dir from shape a to b is possible
int is_compatible(char a, char b, Direction dir)
{
    switch(a)
    {
        case 'F':
            if (dir == UP) return 0;
            if (dir == RIGHT)
                if (b == '7' || b == 'J' || b == '-' || b=='S') return 1;
            if (dir == DOWN)
                if (b == '|' || b == 'J' || b == 'L' || b=='S') return 1;
            if (dir == LEFT) return 0;
            break;

        case '-':
            if (dir == UP) return 0;
            if (dir == RIGHT)
                if (b == '7' || b == 'J' || b == '-' || b=='S') return 1;
            if (dir == DOWN) return 0;
            if (dir == LEFT)
                if (b == 'F' || b == 'L' || b == '-' || b=='S') return 1;
            break;

        case '7':
            if (dir == UP) return 0;
            if (dir == RIGHT) return 0;
            if (dir == DOWN)
                if (b == '|' || b == 'J' || b == 'L' || b=='S') return 1;
            if (dir == LEFT)
                if (b == 'F' || b == 'L' || b == '-' || b=='S') return 1;
            break; 

        case '|':
            if (dir == UP)
                if (b == 'F' || b == '|' || b == '7' || b=='S') return 1;
            if (dir == RIGHT) return 0;
            if (dir == DOWN)
                if (b == '|' || b == 'J' || b == 'L' || b=='S') return 1;
            if (dir == LEFT) return 0;
            break; 

        case 'L':
            if (dir == UP)
                if (b == 'F' || b == '|' || b == '7' || b=='S') return 1;
            if (dir == RIGHT)
                if (b == '-' || b == 'J' || b == '7' || b=='S') return 1;
            if (dir == DOWN) return 0;
            if (dir == LEFT) return 0;
            break; 

        case 'J':
            if (dir == UP)
                if (b == 'F' || b == '|' || b == '7' || b=='S') return 1;
            if (dir == RIGHT) return 0;
            if (dir == DOWN) return 0;
            if (dir == LEFT)
                if (b == '-' || b == 'L' || b == 'F' || b=='S') return 1;
            break;
    }
    return 0;
 }

int is_possible(Maze* maze, int x, int y)
{
    // out of bounds
    if (x == (int)maze->width || x < 0 || y == (int)maze->height || y < 0) return 0;

    int index = y * maze->width + x;

    if (maze->pipes[index].shape == 'S' && // block return to start
        maze->steps < 3)                  // when only if few steps are done.
                                          // this prevents ugly edge-case when
                                          // walker tries to go one step back
                                          // directly to start position
        return 0;

    // this place was already visited
    if (maze->pipes[index].visited &&
        maze->pipes[index].shape != 'S') return 0;

    return 1;
}

char get_pipe(Maze* maze, size_t x, size_t y)
{
    if (! is_possible(maze, x, y)) return '\0';

    int index = y * maze->width + x;
    return maze->pipes[index].shape;
}

void walk_pipe(Maze* maze, size_t x, size_t y)
{
    // calculate 2x area with
    // https://web.archive.org/web/20100405070507/http://valis.cs.uiuc.edu/~sariel/research/CG/compgeom/msg00831.html
    maze->area += maze->prev_x * y;
    maze->area -= maze->prev_y * x;
    maze->prev_x = x;
    maze->prev_y = y;

    int index = y * maze->width + x;
    
    // back to start position! Found loop!
    if (maze->pipes[index].shape == 'S' &&
            maze->pipes[index].visited != 0)
    {
        maze->solved = 1;
        return;
    }

    maze->pipes[index].visited = 1;
    maze->steps++;

    char pipe = maze->pipes[index].shape;

    if (pipe == 'S')
    {
        // at the start determine which moves are possible,
        // pick first that fits
        char t;
        // up
        t = get_pipe(maze, x, y - 1);
        if (t == '|' || t == 'F' || t == '7')
        {
            walk_pipe(maze, x, y - 1);
            return;
        }
        // right
        t = get_pipe(maze, x + 1, y);
        if (t == '-' || t == 'J' || t == '7')
        {
            walk_pipe(maze, x + 1, y);
            return;
        }
        // down
        t = get_pipe(maze, x, y + 1);
        if (t == '|' || t == 'J' || t == 'L')
        {
            walk_pipe(maze, x, y + 1);
            return;
        }
        // left
        t = get_pipe(maze, x - 1, y);
        if (t == '-' || t == 'F' || t == 'L')
        {
            walk_pipe(maze, x - 1, y);
            return;
        }
        printf("Can't start!\n");
    }

    // checks which moves are possible
    // continue recursively when direction works OK

    // up
    if (is_compatible(pipe, get_pipe(maze, x, y - 1), UP))
        walk_pipe(maze, x, y - 1);
    // right
    else if (is_compatible(pipe, get_pipe(maze, x + 1, y), RIGHT))
        walk_pipe(maze, x + 1, y);
    // down
    else if (is_compatible(pipe, get_pipe(maze, x, y + 1), DOWN))
        walk_pipe(maze, x, y + 1);
    // left
    else if (is_compatible(pipe, get_pipe(maze, x - 1, y), LEFT))
        walk_pipe(maze, x - 1, y);
}

void search_maze(Maze* maze)
{
    // search for Start and begin a walk.
    // Walk should result with maze->solved
    for (size_t i = 0; i < maze->size; ++i)
        if (maze->pipes[i].shape == 'S')
        {
            size_t x = i % maze->width;
            size_t y = i / maze->width;
            maze->prev_x = x;
            maze->prev_y = y;
            walk_pipe(maze, x, y);
            if (maze->solved == 1) return;
        }

    printf("Can't find loop");
    return;
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
   
    Maze maze;
    maze.pipes = NULL;
    maze.size = 0;
    maze.solved = 0;
    maze.width = 0;
    maze.height = 0;
    maze.steps = 0;
    maze.area = 0;

    while ((read = getline(&line, &len, input)) != -1)
    {
        maze.height++;
        maze.width = read - 1;

        maze.pipes = (Pipe*) realloc(maze.pipes, (maze.size + maze.width) * sizeof(Pipe));
        if (! maze.pipes)
        {
            fprintf(stderr, "Can't allocate memory");
            fclose(input);
            exit(EXIT_FAILURE);
        }

        for (size_t i = 0; i < maze.width; ++i)
        {
            maze.pipes[maze.size + i].shape = line[i];
            maze.pipes[maze.size + i].visited = 0;
        }
        
        maze.size += maze.width;
    }
    fclose(input);

    search_maze(&maze);
    printf("Steps: %zu\n", maze.steps / 2);

    // https://en.wikipedia.org/wiki/Pick%27s_theorem
    int i = abs(maze.area / 2) - (maze.steps / 2) + 1;
    printf("Integer points interior to the polygon: %d\n", i);

    free(maze.pipes);
    return 0;
}

