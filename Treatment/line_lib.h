/*
 * line_lib.h
 * Data structures and functions for line and column (character) segmentation
 */

#ifndef LINE_LIB_H
#define LINE_LIB_H

#include <stdlib.h>

#include "SDL/SDL.h"

/*
 * struct column - Represents a vertical column (character boundary)
 * @start: Starting x-coordinate
 * @end: Ending x-coordinate
 * @next: Pointer to next column in linked list
 */
struct column
{
  size_t start, end;
  struct column *next;
};

/*
 * struct line - Represents a horizontal line of text
 * @start: Starting y-coordinate
 * @end: Ending y-coordinate
 * @cara: Linked list of character columns within this line
 * @next: Pointer to next line in linked list
 */
struct line
{
  size_t start, end;
  struct column *cara;
  struct line *next;
};

/* Create a new column node for the linked list */
struct column *new_column(size_t start);

/* Create a new line node for the linked list */
struct line *new_line(size_t start);

/* Free a column and all subsequent nodes */
void free_column(struct column *c);

/* Free a line and all its columns and subsequent lines */
void free_line(struct line *l);

/*
 * cutter - Split characters vertically based on gaps within a line
 * @c: Character columns to split
 * @image: Image surface for pixel analysis
 * @y1: Line start y-coordinate
 * @y2: Line end y-coordinate
 *
 * Description:
 *   Analyzes empty vertical columns and splits character regions if gaps
 *   are detected, improving character separation.
 */
void cutter(struct column *c, SDL_Surface *image, size_t y1, size_t y2);

#endif
