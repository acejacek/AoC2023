#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INPUT_FILE "day10.test"

typedef enum { UP, LEFT, DOWN, RIGHT } Direction;

typedef struct
{
    char shape;
    unsigned char visited;
    unsigned char enclosed;
    
} Pipe;

typedef struct
{
    Pipe* pipes;
    size_t width;
    size_t height;
    size_t size;
    size_t solved;
    size_t steps;
} Maze;

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

int is_possible(Maze* maze, size_t x, size_t y)
{
    if (x == maze->width || x < 0 || y == maze->height || y < 0) return 0;

    int index = y * maze->width + x;

    // this place was already visited
    if (maze->pipes[index].visited &&
        maze->pipes[index].shape != 'S')  // allow return to start
        return 0;

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
    }

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

size_t search_maze(Maze* maze)
{
    for (size_t i = 0; i < maze->size; ++i)
        if (maze->pipes[i].shape == 'S')
        {
            size_t x = i % maze->width;
            size_t y = i / maze->width;
            walk_pipe(maze, x, y);
            if (maze->solved == 1) return maze->steps / 2;
        }

    printf("Can't find loop");
    return 0;
}


unsigned char is_visited(Maze* maze, size_t x, size_t y)
{
    int index = y * maze->width + x;
    return maze->pipes[index].visited | maze->pipes[index].enclosed;
}

unsigned int mark_enclosed(Maze* maze, size_t x, size_t y)
{
    if (x == 0 || y == 0 || x == maze->width - 1 || y == maze->height - 1)
        return 0; // reached edge, not encircled

    if (is_visited(maze, x, y)) return 0;
    int index = y * maze->width + x;
    unsigned int enclosed = 1;
    maze->pipes[index].enclosed = enclosed;
    
    if (! is_visited(maze, x, y - 1))
        enclosed = mark_enclosed(maze, x, y - 1);
    if (! is_visited(maze, x, y + 1))
        enclosed += mark_enclosed(maze, x, y + 1);
    if (! is_visited(maze, x - 1, y))
        enclosed += mark_enclosed(maze, x - 1, y);
    if (! is_visited(maze, x + 1, y))
        enclosed += mark_enclosed(maze, x + 1, y);

    enclosed &= 1;

    maze->pipes[index].enclosed = enclosed;

    for (int i = 0; i < maze->size; ++i)
    {
        if (i % maze->width == 0) putchar('\n');
        printf("%c", maze->pipes[i].enclosed+48);
    }
    putchar('\n');
    return enclosed;
}

size_t count_eclosed(Maze* maze)
{
    for (size_t y = 1; y < maze->height - 1; ++y)
        for (size_t x = 1; x < maze->width - 1; ++x)
            mark_enclosed(maze, x, y);

    size_t sum = 0;

    for (size_t i = 0; i < maze->size; ++i)
    {
        sum += maze->pipes[i].enclosed;
    }
    return sum;
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

    for (maze.height = 1;((read = getline(&line, &len, input)) != -1); maze.height++)
    {
        maze.width = read - 1;

        maze.pipes = (Pipe*) realloc(maze.pipes, (maze.size + maze.width) * sizeof(Pipe));
        if (! maze.pipes)
        {
            fprintf(stderr, "Can't allocate memory");
            fclose(input);
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < maze.width; ++i)
        {
            maze.pipes[maze.size + i].shape = line[i];
            maze.pipes[maze.size + i].visited = 0;
            maze.pipes[maze.size + i].enclosed = 0;
        }
        
        maze.size += maze.width;
    }
    fclose(input);

    for (int i = 0; i < maze.size; ++i)
    {
        if (i % maze.width == 0) putchar('\n');
        printf("%c", maze.pipes[i].shape);
    }
    putchar('\n');


    size_t res = search_maze(&maze);
    printf("Steps: %zu\n", res);

    res = count_eclosed(&maze);
    printf("Enclosed: %zu\n", res);
    for (int i = 0; i < maze.size; ++i)
    {
        if (i % maze.width == 0) putchar('\n');
        printf("%c", maze.pipes[i].enclosed+48);
    }
    putchar('\n');

    free(maze.pipes);
    return 0;
}

