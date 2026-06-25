/*
 * word_lib.h
 * Character storage structure and management functions
 */

#ifndef WORD_LIB_H
#define WORD_LIB_H
#include <stdlib.h>

/*
 * struct caracter - Represents a single character as a 16x16 binary matrix
 * @table: 256-element array representing 16x16 pixel grid
 * @next: Pointer to next character in linked list
 */
struct caracter
{
  short table[256];      /* 16x16 bitmap for character representation */
  struct caracter *next; /* Linked list pointer */
};

/* Allocate and initialize a new character node */
struct caracter *new_caracter();

/* Free a character and all subsequent nodes in the list */
void caracter_free(struct caracter *c);

#endif
