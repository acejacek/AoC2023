#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define INPUT_FILE "day21.test"

// change for part 2
#define PART 1
typedef struct plot
{
    long int x;
    long int y;
    struct plot* next;
} Plot;

typedef struct queue
{
    Plot* head;
    Plot* tail;
    size_t count;
} Queue;

void queue_init(Queue* q)
{
    q->head = NULL;
    q->tail = NULL;
    q->count = 0;
}

int enqueue(Queue* q, const size_t x, const size_t y)
{
    Plot* new_plot = malloc(sizeof(Plot));
    if (! new_plot)
    {
        fprintf(stderr, "Error with queue\n");
        return 1;
    }
    new_plot->x = x;
    new_plot->y = y;
    new_plot->next = NULL;

    if (q->tail) q->tail->next = new_plot;
    q->tail = new_plot;

    if (q->head == NULL) q->head = new_plot;

    q->count++;

    return 0;
}

Plot dequeue(Queue* q)
{
    Plot result = { 0, 0 , NULL};
    if (q->head != NULL)
    {
        Plot* p = q->head;
        result = *p;
        free(p);
        q->head = q->head->next;
        if (! q->head) q->tail = NULL;

        q->count--;
    }
    return result; 
}

void show_garden(char* g, const size_t h, const size_t w)
{
    for (size_t y = 0; y < h; ++y)
    {
        for (size_t x = 0; x < w; ++x)
            putchar(g[y * w + x]);
        putchar('\n');
    }
}

size_t count_garden(char* g, const size_t h, const size_t w)
{
    size_t res = 0;
    for (size_t y = 0; y < h; ++y)
    {
        for (size_t x = 0; x < w; ++x)
            if(g[y * w + x] == 'O') res++;
    }
    return res;
}

Plot find_start(char* g, const size_t h, const size_t w)
{
    size_t res = 0;
    for (size_t y = 0; y < h; ++y)
        for (size_t x = 0; x < w; ++x)
            if(g[y * w + x] == 'S') return (Plot){x, y, NULL};

    return (Plot){0, 0, NULL};
}

void check_move(char* g, const size_t h, const size_t w,  Queue* q, Plot p)
{
    long int x = p.x % w;
    long int y = p.y % h;






}

int mark_visited(char** garden, const size_t h, const size_t w)
{
    char* g = *garden;
    char* new_g = calloc(h * w, sizeof(char));
    if (new_g == NULL) return 0;
    for (size_t y = 0; y < h; ++y)
    {
        for (size_t x = 0; x < w; ++x)
        {
            switch(g[y * w + x])
            {
                case 'S':
                case 'O':
                    if (y > 0)
                    {
                        if (g[(y - 1) * w + x] == '.')
                            new_g[(y - 1) * w + x] = 'O';
                    }
                    if (y < h - 1)
                    {
                        if (g[(y + 1) * w + x] == '.')
                            new_g[(y + 1) * w + x] = 'O';
                    }
                    if (x > 0)
                    {
                        if (g[y * w + x - 1] == '.')
                            new_g[y * w + x - 1] = 'O';
                    }
                    if (x < w - 1)
                    {
                        if (g[y* w + x + 1] == '.')
                            new_g[y * w + x + 1] = 'O';
                    }

                    new_g[y * w + x] = '.';

                    break;

                case '.':
                    if (new_g[y * w + x] != 'O')
                        new_g[y * w + x] = '.';

                    break;

                case '#':
                    new_g[y * w + x] = '#';
            }
        }
    }

    free(*garden);
    *garden = new_g;

    return 1;
}

int main(void)
{

#if PART == 1
    const size_t steps = 64;
#else
    const size_t steps = 26501365;
#endif

    char* garden = NULL;

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

    size_t height = 0;
    size_t width = 0;
    for ( height = 0; (read = getline(&line, &len, input)) != -1; ++height)
    {
      if (width ==0) width = read - 1;
      garden = realloc(garden, (height + 1) * width * sizeof(char));
      if (garden == NULL)
      {
        fprintf(stderr, "Can't allocate memory");
        fclose(input);
        exit(EXIT_FAILURE);
      }
      strncpy(garden + (height * width), line, width);
    }

    fclose(input);

    // show_garden(garden, height, width);

    for (size_t s = 0; s < steps; ++s)
    {
        if (! mark_visited(&garden, height, width))
            fprintf(stderr, "Allocation error.");

       // show_garden(garden, height, width);
    }
    
    printf("Garden plots in reach: %zu\n", count_garden(garden, height, width));

    free(garden);
    free(line);
    return 0;
}

