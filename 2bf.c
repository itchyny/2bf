#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DISTANCE(x,y) ((x) > (y) ? ((x) - (y)) : ((y) - (x)))
#define MAX(x,y) ((x) > (y) ? (x) : (y))
#define MIN(x,y) ((x) < (y) ? (x) : (y))
#define ABS(x) ((x) > (0) ? (x) : (-(x)))
#define SQRT(x) (6 + (x) / 24)

typedef struct bf_state {
  int index;
  int max_index;
  unsigned char memory[256];
} bf_state;

void set_memory(bf_state* state, int index, unsigned char after) {
  int i, distance = (int)DISTANCE(state->index, index),
         difference = (int)DISTANCE(state->memory[index], after);
  char c = state->index < index ? '>' : '<';
  for (i = 0; i < distance; ++i) printf("%c", c);
  c = state->memory[state->index = index] < after ? '+' : '-';
  for (i = 0; i < difference; ++i) printf("%c", c);
  state->memory[state->index] = after;
  state->max_index = MIN(MAX(state->max_index, state->index + 1), sizeof(state->memory));
}

void prepare(bf_state* state, unsigned char x, bf_state* newstate, bf_state* lateststate) {
  int i, index, min_length, new_length;
  unsigned char y = SQRT(x);
  min_length = y * (state->max_index ? 2 : 1) - ABS(((int)x) - (int)y * (int)((x + y / 2) / y)) + 2;
  index = state->max_index;
  for (i = 0; i < state->max_index; ++i) {
    if (min_length >= (new_length = DISTANCE(x, lateststate->memory[i]))) {
      index = i;
      min_length = new_length;
    }
  }
  if (index == state->max_index && state->max_index < sizeof(state->memory)) {
    ++state->max_index;
    if (index == 0 && state->max_index < sizeof(state->memory)) {
      ++state->max_index;
      ++index;
    }
    newstate->memory[index] = x;
  }
  lateststate->memory[index] = x;
}

void multiple(bf_state* state, bf_state* newstate) {
  int i, count = 0, loop_count = 0, pre_index = 0, max_index = 0,
      min_index = -2, size = sizeof(state->memory) - 1;
  for (i = 0; i < size; ++i) {
    if (newstate->memory[i] > 13) {
      ++count;
      loop_count += newstate->memory[i];
      max_index = i + 1;
    }
    if (min_index < -1 && newstate->memory[i])
      min_index = i - 1;
  }
  if (count > 1 || (count == 1 && loop_count > 13)) {
    loop_count /= count; loop_count = SQRT(loop_count);
    pre_index = min_index < 0 || (min_index >= 0 && state->memory[min_index] > 0)
              || DISTANCE(state->index, min_index) > DISTANCE(state->index, max_index)
              ? max_index : min_index;
    set_memory(state, pre_index, loop_count);
    printf("[");
    for (i = 0; i < size; ++i) {
      if (newstate->memory[i] >= loop_count) {
        set_memory(state, i, (newstate->memory[i] + loop_count / 2) / loop_count);
        state->memory[i] = loop_count * state->memory[i];
      }
    }
    set_memory(state, pre_index, state->memory[pre_index] - 1);
    state->memory[pre_index] = 0;
    printf("]");
  }
}

void printchar(bf_state* state, unsigned char num) {
  int i, index = 0, new_length, min_length = -1;
  for (i = 0; i < state->max_index; ++i) {
    new_length = DISTANCE(state->index, i) + DISTANCE(state->memory[i], num);
    if (min_length < 0 || min_length > new_length) {
      min_length = new_length;
      index = i;
    }
  }
  set_memory(state, index, num);
  printf(".");
  fflush(stdout);
}

int main(int argc, char *argv[]) {
  unsigned char buffer[64];
  int i, ch, bufferlen;
  FILE* fp = NULL;
  bf_state state = { 0, 0, { 0 } }, initstate = { 0, 0, { 0 } }, newstate, lateststate;
  if (argc >= 2 && strcmp(argv[1], "-") != 0)
    if ((fp = fopen(argv[1], "r")) == NULL) {
      fprintf(stderr, "bf: input file error\n");
      exit(EXIT_FAILURE);
    }
  if (fp == NULL) fp = stdin;
  while (1) {
    for (i = 0; i < sizeof(buffer); ++i)
      if ((ch = fgetc(fp)) != EOF) buffer[i] = (unsigned char)ch;
      else break;
    bufferlen = i; newstate = initstate; lateststate = state;
    for (i = 0; i < bufferlen; ++i)
      prepare(&state, buffer[i], &newstate, &lateststate);
    multiple(&state, &newstate);
    for (i = 0; i < bufferlen; ++i)
      printchar(&state, buffer[i]);
    if (ch == EOF) break;
  }
  printf("\n");
  return 0;
}
