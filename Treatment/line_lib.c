#include "line_lib.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "pixel_operations.h"

static void fatal(const char *msg)
{
  fprintf(stderr, "%s\n", msg);
  exit(EXIT_FAILURE);
}

struct column *new_column(size_t start)
{
  struct column *c = malloc(sizeof(struct column));
  if (c == NULL)
    fatal("No place!");
  c->start = start;
  c->end   = 0;
  c->next  = NULL;
  return c;
}

struct line *new_line(size_t start)
{
  struct line *l = malloc(sizeof(struct line));
  if (l == NULL)
    fatal("No place!");
  l->start = start;
  l->next  = NULL;
  l->cara  = new_column(0);
  return l;
}

void free_column(struct column *c)
{
  if (c != NULL)
  {
    free_column(c->next);
    free(c);
  }
}
void free_line(struct line *l)
{
  if (l != NULL)
  {
    free_column(l->cara);
    free_line(l->next);
    free(l);
  }
}

static bool empty_column(SDL_Surface *image, size_t x, size_t y1, size_t y2)
{
  for (size_t y = y1; y <= y2; ++y)
  {
    Uint8 color = 1;
    SDL_GetRGB(get_pixel(image, x, y), image->format, &color, &color, &color);
    if (color == 0)
      return false;
  }
  return true;
}

void cutter(struct column *c, SDL_Surface *image, size_t y1, size_t y2)
{
  while (c->next != NULL)
  {
    struct column *col       = c->next;
    size_t         width     = col->end - col->start + 1;
    size_t         best_gap  = 0;
    size_t         gap_start = 0;
    size_t         gap_len   = 0;

    for (size_t x = col->start; x <= col->end; ++x)
    {
      if (empty_column(image, x, y1, y2))
      {
        gap_len++;
      }
      else
      {
        if (gap_len > best_gap)
        {
          best_gap  = gap_len;
          gap_start = x - gap_len;
        }
        gap_len = 0;
      }
    }

    if (gap_len > best_gap)
    {
      best_gap  = gap_len;
      gap_start = col->end + 1 - gap_len;
    }

    if (best_gap >= 3 && width > best_gap + 2)
    {
      size_t split_pos = gap_start + best_gap / 2;
      if (split_pos > col->start && split_pos <= col->end)
      {
        struct column *newcol = new_column(split_pos);
        newcol->end           = col->end;
        newcol->next          = col->next;
        col->end              = split_pos - 1;
        col->next             = newcol;
        continue;
      }
    }

    c = c->next;
  }
}
