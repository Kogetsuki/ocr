/*
 * word_lib.c
 * Implementation of character storage structures for OCR pipeline
 * Manages 16x16 binary character matrices for neural network input
 */

#include "word_lib.h"

#include <stdio.h>
#include <stdlib.h>

/*
 * new_caracter - Allocate and initialize a new character node
 *
 * Creates a new character structure with a 16x16 bitmap initialized to zero.
 * This bitmap will be filled with character pixel data during segmentation.
 *
 * Return: Pointer to the new character, or exits on allocation failure.
 */
struct caracter *new_caracter()
{
  struct caracter *c = malloc(sizeof(struct caracter));
  if (c == NULL)
  {
    fprintf(stderr, "No place!\n");
    exit(EXIT_FAILURE);
  }
  c->next = NULL;
  /* Initialize the 16x16 bitmap to all zeros */
  for (size_t i = 0; i < 256; ++i)
    c->table[i] = 0;
  return c;
}

/*
 * caracter_free - Recursively free a character list
 * @c: Character node to free (and all subsequent nodes)
 *
 * Traverses and frees the entire linked list of characters.
 * Safely handles NULL pointer (does nothing if c is NULL).
 */
void caracter_free(struct caracter *c)
{
  if (c == NULL)
    return;
  caracter_free(c->next);
  free(c);
}
