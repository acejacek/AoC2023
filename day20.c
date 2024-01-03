#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INPUT_FILE "day20.txt"

// change for part 2
#define PART 2

typedef enum { UNSPEC, FLIPFLOP, CONJUNCTION } Type;

typedef struct input
{
    int status;
    int ID;
#if PART == 2
    size_t cycle;
#endif
} Input;

typedef struct module
{
    char name[12];
    Type type;
    char output[20][20];   // [FIXME] hard limits here and there
    int status;
    Input inputs[20]; 
    int i_count;
    struct module* output_p[10];
    int o_count;
    int ID;
} Module;

// caries message 
typedef struct signal
{
    int value;
    Module* from;
    Module* to;
    struct signal* next;
} Signal;

// queue of signals
typedef struct queue
{
    Signal* head;
    Signal* tail;
#if PART == 1
    size_t counter_high;
    size_t counter_low;
#else
    Module* parent;
    size_t cycle;
#endif
} Queue;

void init_queue(Queue* q)
{
    q->head = NULL;
    q->tail = NULL;
#if PART == 1
    q->counter_high = 0;
    q->counter_low = 0;
#else
    q->parent = NULL;
    q->cycle = 0;
#endif
}

int enqueue(Queue* q, Module* from, Module* to, const int value)
{
    Signal* new_signal = malloc(sizeof(Signal));
    if (! new_signal)
    {
        fprintf(stderr, "Error with queue\n");
        return 1;
    }

    new_signal->from = from;
    new_signal->to = to;
    new_signal->value = value;
    new_signal->next = NULL;

    if (q->tail) q->tail->next = new_signal;
    q->tail = new_signal;

    if (q->head == NULL) q->head = new_signal;

#if PART == 1
    if (value == 1) ++q->counter_high;
    else ++q->counter_low;
#endif

// for visualization
//     if (from)
//     printf("%s -%d-> %s\n", from->name, value, to->name);
//     else
//     printf("button -%d-> %s\n", value, to->name);

    return 0;
}

Signal dequeue(Queue* q)
{
    Signal result = { 0, NULL, NULL, NULL };
    if (q->head != NULL)
    {
        Signal* s = q->head;
        result = *s;
        free(s);
        q->head = q->head->next;
        if (! q->head) q->tail = NULL;
    }
    return result; 
}

// returns pointer to module with given name
Module* find_module(Module* m, const int i, const char* name)
{
    for (int j = 0; j < i; ++j)
        if (strcmp(name, m[j].name) == 0) return m + j;

    return NULL;
}

// returns pointer to parent of RX module
Module* find_rx_parent(Module* m, const int i)
{
    for (int j = 0; j < i; ++j)
        for (int k = 0; k < m[j].o_count; ++k)
            if (strcmp(m[j].output_p[k]->name, "rx") == 0) return m + j;

    return NULL;
}

// create pointers to output modules
int update_outputs(Module* m, int* count)
{
    int i = *count;   // count of modules can change if we add extra outputs, hence by reference
    for (int j = 0; j < i; ++j)
        for (int k = 0; k < m[j].o_count; ++k)
            if ((m[j].output_p[k] = find_module(m, i, m[j].output[k])) != NULL)  // find all the outputs
            {
                // update outputs:
                int o = m[j].output_p[k]->i_count;
                m[j].output_p[k]->inputs[o].status = 0;   // set default state;
                m[j].output_p[k]->inputs[o].ID = m[j].ID; // link to input
                m[j].output_p[k]->i_count++;
            }
            else
            {
                // create output module, which has no further outputs (end module)
                m = realloc(m, (i + 1) * sizeof(Module));
                if (! m)
                {
                    fprintf(stderr, "Can't allocate memory for additional modules.\n");
                    return 1;
                }

                m[i].type = UNSPEC;
                m[i].o_count = 0;
                m[i].i_count = 0;
                m[i].status = 0;
                m[i].ID = i;
                strcpy(m[i].name, m[j].output[k]);

                m[j].output_p[k] = m + i;  // now when module is created, update parent
                ++i;
            }
    *count = i; // update back to main new module count
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
    int exit_status = 0;
    Module* module = NULL;

    const char filename[] = INPUT_FILE;
    FILE* input = fopen(filename, "r");
    if (!input)
    {
        fprintf(stderr, "Can't load file %s.\n", filename);
        exit_status = EXIT_FAILURE;
        goto dispose;
    }

    char* line = NULL;
    size_t len = 0;

    int i;
    for (i = 0; getline(&line, &len, input) > 2; ++i)
    {
        module = realloc(module, (i + 1) * sizeof(*module));
        if (! module)
        {
            fprintf(stderr, "Can't allocate memory.\n");
            exit_status = EXIT_FAILURE;
            goto dispose;
        }

        module[i].type = UNSPEC;
        module[i].o_count = 0;
        module[i].i_count = 0;
        module[i].status = 0;
        module[i].ID = i;

        int offset = 0;
        if (line[0] == '%')
        {
            module[i].type = FLIPFLOP;
            offset = 1;
        }
        if (line[0] == '&')
        {
            module[i].type = CONJUNCTION;
            offset = 1;
        }

        char* c = strtok(line + offset, " ->\n");
        strcpy(module[i].name, c);
        c = strtok(NULL, "->\n");

        char* d = strtok(c, ", \n");
        do {
            strcpy(module[i].output[module[i].o_count], d);
            module[i].o_count++;
        }
        while ((d = strtok(NULL, ", \n")) != NULL);
    }

    if (update_outputs(module, &i))
    {
        exit_status = EXIT_FAILURE;
        goto dispose;
    }

    Queue q;
    init_queue(&q);

#if PART == 1
    for (int loop = 0; loop < 1000; ++loop)
#else
    q.parent = find_rx_parent(module, i);
    if (q.parent == NULL)
    {
        fprintf(stderr, "Can't find parent module to RX\n");
        exit_status = EXIT_FAILURE;
        goto dispose;
    }

    int found = 0;
    for (q.cycle = 1; ! found; ++q.cycle)
#endif
    {
        // button sends low message to broadcaster
        if (enqueue(&q, NULL, find_module(module, i, "broadcaster"), 0))
        {
            exit_status = EXIT_FAILURE;
            goto dispose;
        }

        // process until queue is empty
        do {
            Signal s = dequeue(&q);
            switch(s.to->type)
            {
                case UNSPEC:
                    // simply forward signal value to all outputs
                    for (int j = 0; j < s.to->o_count; ++j)
                        if (enqueue(&q, s.to, s.to->output_p[j], s.value))
                        {
                            exit_status = EXIT_FAILURE;
                            goto dispose;
                        }
                    break;

                case FLIPFLOP:
                    if (s.value == 1) break; // ignore high impuls;
      
                    s.to->status ^= 1;  // flip state;
                    for (int j = 0; j < s.to->o_count; ++j) // forward state to all outputs
                        if (enqueue(&q, s.to, s.to->output_p[j], s.to->status))
                        {
                            exit_status = EXIT_FAILURE;
                            goto dispose;
                        }
                    break;

                case CONJUNCTION:
                    // update input
                    for (int j = 0; j < s.to->i_count; ++j)
                        if (s.to->inputs[j].ID == s.from->ID)
                        {
                            s.to->inputs[j].status = s.value;
#if PART == 2
                            // for part 2 record cycle, when parent module input get 1
                            if (s.to == q.parent && s.value == 1 && s.to->inputs[j].cycle == 0)   
                                s.to->inputs[j].cycle = q.cycle;
#endif
                            break;
                        }

                    // check status of all inputs
                    s.to->status = 0;  // 0 when all inputs in w 1
                    for (int j = 0; j < s.to->i_count; ++j) 
                        if (s.to->inputs[j].status == 0)
                        {
                            s.to->status = 1;  // 1, when some of inputs are 0
                            break;
                        }

                    // forward status to all outputs
                    for (int j = 0; j < s.to->o_count; ++j)
                        if (enqueue(&q, s.to, s.to->output_p[j], s.to->status))
                        {
                            exit_status = EXIT_FAILURE;
                            goto dispose;
                        }
                    break;

                default:
                    printf("Unknown signal");
                    exit_status = EXIT_FAILURE;
                    goto dispose;
            }
        } while (q.head != NULL);

#if PART == 2
        // check, if cycle calculated for all inputs of parent module to RX
        found = 1;
        for (int j = 0; j < q.parent->i_count; ++j)
            if (q.parent->inputs[j].cycle == 0)
            {
                found = 0;
                break;
            }
#endif
    }

#if PART == 1
    printf("Number of high impulses sent: %zu, low: %zu, total score %zu\n", q.counter_high, q.counter_low, q.counter_high * q.counter_low); 
#else
    // calculate LCM of cycles from all inputs
    // brute force method will take days
    size_t res = 1;
    for (int j = 0; j < q.parent->i_count; ++j)
        res = lcm(res, q.parent->inputs[j].cycle);
        
    printf("Number of button presses until low signal to RX: %zu\n", res); 
#endif

dispose:
    if (input) fclose(input);
    free(module);
    exit(exit_status);
}

