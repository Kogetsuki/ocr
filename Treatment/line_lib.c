#include "line_lib.h"
#include <err.h>
#include <stdio.h>

struct column *new_column(size_t start) {
    struct column *c = malloc(sizeof(struct column));
    if (c == NULL) errx(1, "No place!");
    c->start = start;
    c->end = 0;
    c->next = NULL;
    return c;
}

struct line *new_line(size_t start) {
    struct line *l = malloc(sizeof(struct line));
    if (l == NULL) errx(1, "No place!");
    l->start = start;
    l->next = NULL;
    l->cara = new_column(0);
    return l;
}

void free_column(struct column *c) {
    if (c != NULL)
    {
        free_column(c->next);
        free(c);
    }
}
void free_line(struct line *l) {
    if (l != NULL)
    {
        free_column(l->cara);
        free_line(l->next);
        free(l);
    }
}

void cutter(struct column *c) {
    if (c->next == NULL) return;
    size_t max = c->next->end-c->next->start;
    while (c->next != NULL)
    {
        c = c->next;
        size_t act = c->end-c->start;
        if (act >= 2*max)
        {
            struct column *col = new_column((c->start+c->end)/2);
            col->end = c->end;
            c->end = col->start;
            col->next = c->next;
            c->next = col;
        }
        else if (act > max) max = act-1;
    }
}
