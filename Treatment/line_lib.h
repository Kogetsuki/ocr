#ifndef LINE_LIB_H
#define LINE_LIB_H

#include <stdlib.h>

#include "SDL/SDL.h"

struct column
{
  size_t         start, end;
  struct column *next;
};

struct line
{
  size_t         start, end;
  struct column *cara;
  struct line   *next;
};

struct column *new_column(size_t start);
struct line   *new_line(size_t start);
void           free_column(struct column *c);
void           free_line(struct line *l);
void           cutter(struct column *c, SDL_Surface *image, size_t y1, size_t y2);

#endif
