#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INPUT_FILE "day19.txt"

typedef struct
{
  int min;
  int max;
} Range;

typedef struct Part
{
  int x;
  int m;
  int a;
  int s;
} Part;

struct workflow;

typedef struct
{
  char category;
  char comp;
  unsigned int treshold;
  char destination[5];
  struct workflow* dest_prt;
} Rule;

typedef struct workflow
{
  char name[5];
  Rule rule[10];
  int r;
} Workflow;

int find_workflow(Workflow* workflow, int w, char* name)
{
  for (int i = 0; i < w; ++i)
    if (strcmp(workflow[i].name, name) == 0) return i;

  return -1;
}

int process(Part* part, Workflow* workflow, int r)
{
  int t = workflow->rule[r].treshold;
  int result = 0;
  int success = 0;
  switch (workflow->rule[r].comp)
  {
    case '>':
      switch (workflow->rule[r].category)
      {
        case 'x':
          if (part->x > t) success = 1;
          break;
        case 'm':
          if (part->m > t) success = 1;
          break;
        case 'a':
          if (part->a > t) success = 1;
          break;
        case 's':
          if (part->s > t) success = 1;
          break;
      }
      break;
    case '<':
      switch (workflow->rule[r].category)
      {
        case 'x':
          if (part->x < t) success = 1;
          break;
        case 'm':
          if (part->m < t) success = 1;
          break;
        case 'a':
          if (part->a < t) success = 1;
          break;
        case 's':
          if (part->s < t) success = 1;
          break;
      }
      break;
    case '=':
      success = 1;
      break;
    default:
      printf("Unknown case");
  }
  if (! success) result = process(part, workflow, r + 1);
  else
  {
    if (workflow->rule[r].destination[0] == 'R') return 0;
    if (workflow->rule[r].destination[0] == 'A') return 1;
    result = process(part, workflow->rule[r].dest_prt, 0); // follow next workflow
  }
  return result;
}

size_t process_range(Workflow* workflow, int r, Range x, Range m, Range a, Range s)
{
  size_t result = 0;
  int success = 0;
  int t = workflow->rule[r].treshold;
  switch (workflow->rule[r].comp)
  {
    case '>':                              // greater than treshold
      switch (workflow->rule[r].category)
      {
        case 'x':
          if (x.min > t) success = 1;      // all range is greater than treshold
          else if (x.max > t)    // only upper part
          {
            // prepare new range and inject it here recursively, so it will pass
            result += process_range(workflow, r, (Range){t + 1, x.max}, m, a, s);
            x.max = t; // truncate remaining part for further processing.
          }
          break;
        case 'm':
          if (m.min > t) success = 1;
          else if (m.max > t)
          {
            result += process_range(workflow, r, x,(Range){t + 1, m.max}, a, s);
            m.max = t;
          }
          break;
        case 'a':
          if (a.min > t) success = 1;
          else if (a.max > t)
          {
            result += process_range(workflow, r, x, m, (Range){t + 1, a.max}, s);
            a.max = t;
          }
          break;
        case 's':
          if (s.min > t) success = 1;
          else if (s.max > t)
          {
            result += process_range(workflow, r, x, m, a, (Range){t + 1, s.max});
            s.max = t;
          }
          break;
      }
      break;
    case '<':
      switch (workflow->rule[r].category)
      {
        case 'x':
          if (x.max < t) success = 1;
          else if (x.min < t)
          {
            result += process_range(workflow, r, (Range){x.min, t - 1}, m, a, s);
            x.min = t;
          }
          break;
        case 'm':
          if (m.max < t) success = 1;
          else if (m.min < t)
          {
            result += process_range(workflow, r, x, (Range){m.min, t - 1}, a, s);
            m.min = t;
          }
          break;
        case 'a':
          if (a.max < t) success = 1;
          else if (a.min < t)
          {
            result += process_range(workflow, r, x, m, (Range){a.min, t - 1}, s);
            a.min = t;
          }
          break;
        case 's':
          if (s.max < t) success = 1;
          else if (s.min < t)
          {
            result += process_range(workflow, r, x, m, a, (Range){s.min, t - 1});
            s.min = t;
          }
          break;
      }
      break;
    case '=':
      success = 1;
      break;
    default:
      printf("Unknown case");
  }

  if (! success) result += process_range(workflow, r + 1, x, m, a, s); // process next rule
  else
  {
    if (workflow->rule[r].destination[0] == 'R') return 0;   // dump into sink
    if (workflow->rule[r].destination[0] == 'A')             // accept
    {
      result += (size_t)(x.max - x.min + 1) * (size_t)(m.max - m.min + 1) * (size_t)(a.max - a.min + 1) * (size_t)(s.max - s.min + 1);
      return result;
    }
    // destination is next rule
    result += process_range(workflow->rule[r].dest_prt, 0, x, m, a, s); // follow next workflow
  }
  return result;
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

  Workflow* workflow = NULL;
  int i = 0;

  char* line = NULL;
  size_t len = 0;
  ssize_t read = 0;

  while ((read = getline(&line, &len, input)) > 2)
  {
    workflow = realloc(workflow, (i + 1) * sizeof(Workflow));
    if (! workflow)
    {
      fprintf(stderr, "Can't allocate memory.\n");
      fclose(input);
      exit(EXIT_FAILURE);
    }
    char* c = strtok(line, "{}\n");
    strcpy(workflow[i].name, c);

    char rules[100] = { 0 };
    int r = 0;
    c = strtok(NULL, "{}\n");
    strcpy(rules, c);
    c = strtok(rules, ",");
    do {
      switch(c[1])
      {
        case '<':
        case '>':
          workflow[i].rule[r].category = c[0];
          workflow[i].rule[r].comp = c[1];
          workflow[i].rule[r].treshold = atoi(c + 2);
          char* x = strchr(c, ':');
          strcpy(workflow[i].rule[r].destination, x + 1);
          break;
        default:
          workflow[i].rule[r].comp = '=';
          strcpy(workflow[i].rule[r].destination, c);
      }
      workflow[i].r = ++r;
    }
    while ((c = strtok(NULL, ",")) != NULL);
    i++;
  }

  Part* part = NULL;
  int p = 0;

  while (getline(&line, &len, input) != -1)
  {
    part = realloc(part, (i + 1) * sizeof(Part));
    if (! part)
    {
      fprintf(stderr, "Can't allocate memory.\n");
      fclose(input);
      exit(EXIT_FAILURE);
    }
    char* c = strtok(line, "{,}\n");
    do {
      int val = atoi(c + 2);
      switch(c[0])
      {
        case 'x':
          part[p].x = val;
          break;
        case 'm':
          part[p].m = val;
          break;
        case 'a':
          part[p].a = val;
          break;
        case 's':
          part[p].s = val;
          break;
      }
    }
    while ((c = strtok(NULL, ",")) != NULL);
    p++;
  }

  for (int n = 0; n < i; ++n)
  {
    for (int m = 0; m < workflow[n].r; ++m)
    {
      int index = find_workflow(workflow, i, workflow[n].rule[m].destination);
      if (index != -1) workflow[n].rule[m].dest_prt = workflow + index;
    }
  }

  // work out all parts
  int start = find_workflow(workflow, i, "in");
  if (start < 0) goto dispose;

  size_t score = 0;
  for (int n = 0; n < p; ++n)
    if (process(part + n,  workflow + start, 0))
        score += part[n].x + part[n].m + part[n].a + part[n].s;

  printf("Result part 1: %zu\n", score); 

  Range r = {1, 4000};
  score = process_range(workflow + start,  0, r, r, r, r);

  printf("Result part 2: %zu\n", score); 

dispose:
  fclose(input);
  free(workflow);
  free(part);
  return 0;
}

