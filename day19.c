#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INPUT_FILE "day19.test"

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
  int treshold;
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
  int result = 0;
  int success = 0;
  switch (workflow->rule[r].comp)
  {
    case '>':
      switch (workflow->rule[r].category)
      {
        case 'x':
          if (part->x > workflow->rule[r].treshold) success = 1;
          break;
        case 'm':
          if (part->m > workflow->rule[r].treshold) success = 1;
          break;
        case 'a':
          if (part->a > workflow->rule[r].treshold) success = 1;
          break;
        case 's':
          if (part->s > workflow->rule[r].treshold) success = 1;
          break;
      }
      break;
    case '<':
      switch (workflow->rule[r].category)
      {
        case 'x':
          if (part->x < workflow->rule[r].treshold) success = 1;
          break;
        case 'm':
          if (part->m < workflow->rule[r].treshold) success = 1;
          break;
        case 'a':
          if (part->a < workflow->rule[r].treshold) success = 1;
          break;
        case 's':
          if (part->s < workflow->rule[r].treshold) success = 1;
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
    result = process(part, workflow->rule[r].dest_prt, 0); // follow next rule
  }
  return result;
}

int process_range(Workflow* workflow, int r, Range x, Range m, Range a, Range s)
{
  int result = 0;
  int success = 0;
  int t = workflow->rule[r].treshold;
  switch (workflow->rule[r].comp)
  {
    case '>':
      switch (workflow->rule[r].category)
      {
        case 'x':
          if (x.min > t) success = 1;
          else if (x.max > t)
            result += process_range(workflow, r, (Range){t, x.max}, m, a, s);
          break;
        case 'm':
          if (m.min > t) success = 1;
            result += process_range(workflow, r, x,(Range){t, m.max}, a, s);
          break;
        case 'a':
          if (a.min > t) success = 1;
            result += process_range(workflow, r, x, m, (Range){t, a.max}, s);
          break;
        case 's':
          if (s.min > t) success = 1;
            result += process_range(workflow, r, x, m, a, (Range){t, x.max});
          break;
      }
      break;
    case '<':
      switch (workflow->rule[r].category)
      {
        case 'x':
          if (x.max < t) success = 1;
          else if (x.min > t)
            result += process_range(workflow, r, (Range){x.min, t}, m, a, s);
          break;
        case 'm':
          if (m.max < t) success = 1;
          else if (m.min > t)
            result += process_range(workflow, r, (Range){x.min, t}, m, a, s);
          break;
        case 'a':
          if (a.max < t) success = 1;
          else if (a.min > t)
            result += process_range(workflow, r, (Range){x.min, t}, m, a, s);
          break;
        case 's':
          if (x.max < t) success = 1;
          else if (s.min > t)
            result += process_range(workflow, r, (Range){x.min, t}, m, a, s);
          break;
      }
      break;
    case '=':
      success = 1;
      break;
    default:
      printf("Unknown case");
  }
  if (! success) result = process_range(workflow, r + 1, x, m, a, s);
  else
  {
    if (workflow->rule[r].destination[0] == 'R') return 0;
    if (workflow->rule[r].destination[0] == 'A') return 1;
    result += process_range(workflow->rule[r].dest_prt, 0, x, m, a, s); // follow next rule
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
//    printf("Flow: %s, rules %s\n", workflow[i].name, rules);
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
//    printf("Flow: %s, %d rules:\n", workflow[n].name, workflow[n].r);
    for (int m = 0; m < workflow[n].r; ++m)
    {
      int index = find_workflow(workflow, i, workflow[n].rule[m].destination);
      if (index != -1) workflow[n].rule[m].dest_prt = workflow + index;
//      printf("cat: %c, crit: %c, treshold: %d, dest: %s ptr: %p\n",
//          workflow[n].rule[m].category, workflow[n].rule[m].comp,
//          workflow[n].rule[m].treshold, workflow[n].rule[m].destination,
//          (void*)workflow[n].rule[m].dest_prt);
    }
  }

//  for (int n = 0; n < p; ++n)
//    printf("x: %d, m: %d, a: %d, s: %d\n", part[n].x, part[n].m, part[n].a, part[n].s);

  // work out all parts
  int start = find_workflow(workflow, i, "in");
  if (start < 0) exit(EXIT_FAILURE);

  size_t score = 0;
  for (int n = 0; n < p; ++n)
    if (process(part + n,  workflow + start, 0))
        score += part[n].x + part[n].m + part[n].a + part[n].s;

  printf("Result part 1: %zu\n", score); 

  score = 0;

  




  printf("Result part 2: %zu\n", score); 


  fclose(input);
  free(workflow);
  free(part);
  return 0;
}

