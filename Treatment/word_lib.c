#include "word_lib.h"

#include <stdio.h>
#include <stdlib.h>

struct caracter *new_caracter()
{
  struct caracter *c = malloc(sizeof(struct caracter));
  if (c == NULL)
  {
    fprintf(stderr, "No place!\n");
    exit(EXIT_FAILURE);
  }
  c->next = NULL;
  for (size_t i = 0; i < 256; ++i)
    c->table[i] = 0;
  return c;
}

void caracter_free(struct caracter *c)
{
  if (c == NULL)
    return;
  caracter_free(c->next);
  free(c);
}
