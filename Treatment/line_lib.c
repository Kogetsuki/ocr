/*
 * line_lib.c
 * Implementation of line and character column segmentation structures
 * Provides linked list management and character boundary detection
 */

#include "line_lib.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "pixel_operations.h"

/*
 * fatal - Print error message and exit program
 * @msg: Error message to display
 *
 * Helper function for handling fatal allocation errors.
 * Meant to replace errx for Windows
 */
static void fatal(const char *msg)
{
  fprintf(stderr, "%s\n", msg);
  exit(EXIT_FAILURE);
}

/*
 * new_column - Create and initialize a new column node
 * @start: Starting x-coordinate of the column
 *
 * Allocates memory for a column structure and initializes its fields.
 * Return: Pointer to the new column, or exits on allocation failure.
 */
struct column *new_column(size_t start)
{
  struct column *c = malloc(sizeof(struct column));
  if (c == NULL)
    fatal("No place!");
  c->start = start;
  c->end = 0;
  c->next = NULL;
  return c;
}

/*
 * new_line - Create and initialize a new line node
 * @start: Starting y-coordinate of the line
 *
 * Allocates memory for a line structure, initializes its fields,
 * and creates an initial column list.
 * Return: Pointer to the new line, or exits on allocation failure.
 */
struct line *new_line(size_t start)
{
  struct line *l = malloc(sizeof(struct line));
  if (l == NULL)
    fatal("No place!");
  l->start = start;
  l->next = NULL;
  l->cara = new_column(0); /* Initialize with a dummy column */
  return l;
}

/*
 * free_column - Recursively free a column list
 * @c: Column node to free (and all subsequent nodes)
 *
 * Traverses and frees the entire linked list of columns.
 */
void free_column(struct column *c)
{
  if (c != NULL)
  {
    free_column(c->next);
    free(c);
  }
}

/*
 * free_line - Recursively free a line and all its data
 * @l: Line node to free (and all subsequent lines)
 *
 * Traverses and frees the entire linked list of lines, including
 * all columns within each line.
 */
void free_line(struct line *l)
{
  if (l != NULL)
  {
    free_column(l->cara);
    free_line(l->next);
    free(l);
  }
}

/*
 * empty_column - Check if a vertical column contains only white pixels
 * @image: The image surface to analyze
 * @x: X-coordinate of the column to check
 * @y1: Starting y-coordinate of the range
 * @y2: Ending y-coordinate of the range
 *
 * Return: true if column is entirely white (empty), false if contains black
 *
 * Used to detect vertical spaces between characters.
 */
static bool empty_column(SDL_Surface *image, size_t x, size_t y1, size_t y2)
{
  for (size_t y = y1; y <= y2; ++y)
  {
    Uint8 color = 1;
    SDL_GetRGB(get_pixel(image, x, y), image->format, &color, &color, &color);
    if (color == 0) /* Found black pixel - column is not empty */
      return false;
  }
  return true; /* No black pixels found - column is empty */
}

/*
 * cutter - Split character columns based on gaps
 * @c: Column list to process and potentially split
 * @image: Image surface for pixel analysis
 * @y1: Top of the line
 * @y2: Bottom of the line
 *
 * Analyzes each character column to find the largest gap of empty columns.
 * If a significant gap exists (>= 3 pixels), splits the character region
 * into two separate columns to improve character separation.
 *
 * This is crucial for correctly handling merged characters or preventing
 * characters from being treated as a single entity.
 */
void cutter(struct column *c, SDL_Surface *image, size_t y1, size_t y2)
{
  while (c->next != NULL)
  {
    struct column *col = c->next;
    size_t width = col->end - col->start + 1;
    size_t best_gap = 0;  /* Largest gap found so far */
    size_t gap_start = 0; /* Position of largest gap */
    size_t gap_len = 0;   /* Current gap length */

    /* Scan through the column looking for empty vertical strips */
    for (size_t x = col->start; x <= col->end; ++x)
    {
      if (empty_column(image, x, y1, y2))
      {
        gap_len++;
      }
      else
      {
        /* End of current gap - check if it's the largest so far */
        if (gap_len > best_gap)
        {
          best_gap = gap_len;
          gap_start = x - gap_len;
        }
        gap_len = 0;
      }
    }

    /* Check final gap at the end of the column */
    if (gap_len > best_gap)
    {
      best_gap = gap_len;
      gap_start = col->end + 1 - gap_len;
    }

    /* If we found a significant gap and the column is wide enough, split it */
    if (best_gap >= 3 && width > best_gap + 2)
    {
      size_t split_pos = gap_start + best_gap / 2;
      if (split_pos > col->start && split_pos <= col->end)
      {
        /* Create new column for the right portion */
        struct column *newcol = new_column(split_pos);
        newcol->end = col->end;
        newcol->next = col->next;
        col->end = split_pos - 1;
        col->next = newcol;
        continue; /* Process the new column immediately */
      }
    }

    c = c->next;
  }
}
