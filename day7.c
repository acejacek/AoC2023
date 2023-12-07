#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INPUT_FILE "day7.txt"

// change for part 2
#define PART 1

typedef enum { FIVE, FOUR, FULL, THREE, TWO_PAIRS, PAIR, HIGH_CARD } Type;

typedef struct
{
    char cards[6];
    char backup[6];
    size_t bid;
    Type type;
} Hand;

int compare_cards(char* p, char* q)
{
#if PART == 1
    char all_cards[] = "AKQJT98765432.";
#else
    char all_cards[] = "AKQT98765432J.";
#endif

    int t1 = 0;
    int t2 = 0;
    for (int i = 0; i < 14; ++i)
    {
        if (*p == all_cards[i]) t1 = i;
        if (*q == all_cards[i]) t2 = i;
    }

    return t2 - t1;
}

// compare function, which is qsort-compatible
int compare_hands(const void * a, const void * b)
{
    Hand* p = (Hand*) a;
    Hand* q = (Hand*) b;

    // check on type
    if (p->type < q->type) return 1;    
    if (p->type > q->type) return -1;

    // if equal type, compare cards until diff
    int res = 0;
    for (int i = 0; i < 6; ++i)
    {
        res = compare_cards(p->backup + i, q->backup + i);
        if (res !=0 ) break;
    }

    return res;
}

void sort_hands(Hand* hands, const size_t i)
{
    qsort(hands, i, sizeof(Hand), &compare_hands);
}

// universal function, which searches n identical cards in hand
int has_n(Hand* h, const int n)
{
#if PART == 1
    char all_cards[] = "AKQJT98765432";
#else
    char all_cards[] = "AKQT98765432J";
#endif

    for (char* cards = all_cards; *cards != '\0'; ++cards)
    {
        int c = 0;
        for (int i = 0; i < 6; ++i)
        {
            if (h->cards[i] == *cards) c++;
#if PART == 2
            // use joker!
            else if (h->cards[i] == 'J') c++;
#endif
        }
        if (c == n)
        {
            // remove found cards from hand, so lower level search will not
            // find them again
            for (int j = 0; j < 6; ++j)
            {
                if (h->cards[j] == *cards) h->cards[j] = '.';
#if PART == 2            
                // if there was a joker, for sure took part in creation
                if (h->cards[j] == 'J') h->cards[j] = '.';
#endif
            }
            return 1;
        }
    }
    return 0;
}

int has_five(Hand* h)
{
    if (has_n(h, 5))
    {
        h->type = FIVE;
        return 1;
    }
    return 0;
}

int has_four(Hand* h)
{
    if (has_n(h, 4))
    {
        h->type = FOUR;
        return 1;
    }
    return 0;
}

int has_three(Hand* h)
{
    if (has_n(h, 3))
    {
        h->type = THREE;
        // is there also a pair?
        if (has_n(h, 2))
            h->type = FULL;

        return 1;
    }
    return 0;
}

int has_pair(Hand* h)
{
    if (has_n(h, 2))
    {
        h->type = PAIR;
        // is there a second pair?
        if (has_n(h, 2))
            h->type = TWO_PAIRS;
        return 1;
    }
    return 0;
}

int main(void)
{
    Hand* hands = NULL;

    const char filename[] = INPUT_FILE;
    FILE* input = fopen(filename, "r");
    if (!input)
    {
        fprintf(stderr, "Can't load file %s.\n", filename);
        exit(EXIT_FAILURE);
    }

    char* line = NULL;
    size_t len = 0;
    size_t h;
    for (h = 0; getline(&line, &len, input) != -1; ++h)
    {
        hands = realloc(hands, (h + 1) * sizeof(Hand));
        if (! hands)
        {
            fprintf(stderr, "Can't allocate memory");
            fclose(input);
            exit(EXIT_FAILURE);
        }

        sscanf(line, "%s %zu", hands[h].cards, &hands[h].bid);

        hands[h].type = HIGH_CARD;  // assume, there is nothing special in hand

        for (int i = 0; i < 6; ++i)
            hands[h].backup[i] = hands[h].cards[i]; // needed for scoring and sorting

        // determine what's in the hand
        // sequence of calls is important (strongest -> lowest)

        if (! has_five(hands + h))             // FIVE?
            if (! has_four(hands + h))         // FOUR?
                if (! has_three(hands + h))    // FULL or THREE?
                        has_pair(hands + h);   // TWO_PAIRS or PAIR?
    }

    fclose(input);
   
    // sort hands from weakest to strongest 
    sort_hands(hands, h);

    size_t total = 0;
    for (size_t i = 0; i < h; ++i)
        total += (i + 1) * hands[i].bid;

    printf("Winning: %zu\n", total);

    free(hands);
    return 0;
}

