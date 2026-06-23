#ifndef WORD_LIB_H
#define WORD_LIB_H
#include <stdlib.h>

struct caracter {
    short table[256];
    struct caracter *next;
};

struct caracter *new_caracter();
void caracter_free(struct caracter *c);

#endif
