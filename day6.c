#include <stdio.h>

// change for part 2
#define PART 1

typedef struct
{
    size_t time;
    size_t distance;
    size_t ways_to_win;
} Race;

size_t simulate_race(const size_t time, const size_t button_time)
{
     return (time - button_time) * button_time; 
}

int main(void)
{
#if PART == 1

    Race races[] =
    {
        [0].time = 62,
        [0].distance = 644,

        [1].time = 73,
        [1].distance = 1023,

        [2].time = 75,
        [2].distance = 1240,

        [3].time = 65,
        [3].distance = 1023,
    };

    int race_count = 4;

#else

    Race races[] =
    {
        [0].time =  62737565,
        [0].distance =  644102312401023,
    };

    int race_count = 1;

#endif

    size_t result = 1;

    for (int i = 0; i < race_count; ++i)
    {
        for (size_t button_time = 1; button_time <= races[i].time; ++button_time)
        {
            size_t sim_distance = simulate_race(races[i].time, button_time);
            if (sim_distance > races[i].distance)
                races[i].ways_to_win++;
        }
        result *= races[i].ways_to_win;
    }

    printf("Result: %zu\n", result);

    return 0;
}

