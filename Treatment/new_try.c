/*
 * new_try.c
 * Character segmentation and extraction from binary images
 * Implements the core OCR pipeline: line detection -> character separation ->
 * bitmap normalization -> neural network input generation
 */

#include "new_try.h"

#include <stdlib.h>

#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "line_lib.h"
#include "pixel_operations.h"
#include "word_lib.h"

/*
 * columns - Detect character columns (vertical boundaries) within a line
 * @image: Binary image surface
 * @line: Line structure to fill with column data
 * @width: Width of the image
 *
 * Scans horizontally through the specified line to identify vertical regions
 * containing black pixels. Each contiguous region becomes a column representing
 * a single character.
 */
void columns(SDL_Surface *image, struct line *line, size_t width)
{
  int ended = 1; /* Track whether we're inside a character (black region) */
  struct column *curr = line->cara;
  for (size_t i = 0; i < width; ++i)
  {
    Uint8 color = 1; /* Default to white */
    /* Scan vertically at x position to check if there's any black pixel */
    for (size_t j = line->start; color != 0 && j < line->end; ++j)
    {
      SDL_GetRGB(get_pixel(image, i, j), image->format, &color, &color, &color);
    }
    /* Transition from white to black - start of a character column */
    if (color == 0 && ended)
    {
      curr->next = new_column(i);
      curr = curr->next;
      ended = 0;
    }
    /* Transition from black to white - end of a character column */
    else if (color != 0 && !ended)
    {
      curr->end = i - 1;
      ended = 1;
    }
  }
  /* Handle case where line ends while still in a character */
  if (!ended)
    curr->end = width - 1;
}

/*
 * lines - Detect text lines (horizontal regions) in the image
 * @image: Binary image surface
 * @l: Line structure to populate with detected lines
 *
 * Scans vertically through the image to identify horizontal regions containing
 * black pixels. For each detected line, also detects its character columns.
 */
void lines(SDL_Surface *image, struct line *l)
{
  size_t height = image->h;
  size_t width = image->w;
  int ended = 1; /* Track whether we're inside a line */
  struct line *curr = l;
  for (size_t i = 0; i < height; ++i)
  {
    Uint8 color = 1; /* Default to white */
    /* Scan horizontally at y position to check for any black pixel */
    for (size_t j = 0; color != 0 && j < width; ++j)
    {
      SDL_GetRGB(get_pixel(image, j, i), image->format, &color, &color, &color);
    }
    /* Transition from white to black - start of a text line */
    if (color == 0 && ended)
    {
      curr->next = new_line(i);
      curr = curr->next;
      ended = 0;
    }
    /* Transition from black to white - end of a text line */
    else if (color != 0 && !ended)
    {
      curr->end = i - 1;
      ended = 1;
      /* Detect character columns within this line */
      columns(image, curr, width);
    }
  }
  /* Handle case where image ends while still in a line */
  if (!ended)
  {
    curr->end = height - 1;
    columns(image, curr, width);
  }
}

/*
 * colcar - Resize character bitmap to 16x16 neural network format
 * @y1: Top of the character region
 * @y2: Bottom of the character region
 * @col: Character column boundaries
 * @car: Character structure to fill with 16x16 bitmap
 * @image: Binary image surface
 *
 * Maps the original character pixel region to a 16x16 normalized bitmap.
 * Uses bilinear interpolation-like scaling to preserve character features.
 * The character is centered in the 16x16 space to handle aspect ratio differences.
 */
void colcar(size_t y1, size_t y2, struct column *col, struct caracter *car, SDL_Surface *image)
{
  /* Determine the larger dimension (width or height) for scaling */
  size_t max = y2 - y1 < col->end - col->start ? col->end + 1 - col->start : y2 + 1 - y1;
  /* Calculate centering offsets to maintain aspect ratio */
  size_t xstart = (max - col->end + col->start) / 2;
  size_t ystart = (max - y2 + y1) / 2;
  /* Scaling factor: fit original size to 16 pixels */
  float passage = 16. / (float) max;

  /* Process each pixel in the original character region */
  for (size_t x = col->start; x <= col->end; ++x)
  {
    for (size_t y = y1; y <= y2; ++y)
    {
      Uint8 color;
      SDL_GetRGB(get_pixel(image, x, y), image->format, &color, &color, &color);
      /* Only process black pixels (character content) */
      if (color == 0)
      {
        /* Calculate target 16x16 coordinates with scaling and centering */
        size_t x1car = passage * (x - col->start + xstart);
        size_t x2car = passage * (x + 1 - col->start + xstart);
        size_t y1car = passage * (y - y1 + ystart);
        size_t y2car = passage * (y + 1 - y1 + ystart);
        /* Mark corresponding cells in the 16x16 bitmap */
        for (size_t xcar = x1car; xcar < x2car; ++xcar)
          for (size_t ycar = y1car; ycar < y2car; ++ycar)
            car->table[xcar + ycar * 16] = 1;
      }
    }
  }
}

/*
 * print_car - Output a character bitmap in human-readable format
 * @file: File handle for output
 * @c: Character to print
 *
 * Prints the 16x16 bitmap as a visual representation for debugging:
 * 'x' for black pixels, space for white pixels.
 */
void print_car(FILE *file, struct caracter *c)
{
  for (size_t i = 0; i < 16; ++i)
  {
    for (size_t j = 0; j < 16; ++j)
    {
      fprintf(file, c->table[i * 16 + j] ? "x" : " ");
    }
    fprintf(file, "\n");
  }
  fprintf(file, "\n\n");
}

/*
 * lisent - Process all characters and generate neural network input
 * @l: Line structure containing all detected lines and characters
 * @image: Binary image surface
 *
 * Main processing function that:
 *   1. Iterates through all lines
 *   2. Applies character-splitting algorithm (cutter)
 *   3. Calculates average character width for space detection
 *   4. Processes each character to 16x16 format
 *   5. Outputs neural network training data to .car file
 *   6. Outputs visual representations to .out file
 *   7. Detects and marks word spaces
 *
 * Output files:
 *   - .car: Binary representations (1s and 0s) for neural network input
 *   - .out: Visual representations (x and space) for debugging
 */
void lisent(struct line *l, SDL_Surface *image)
{
  FILE *file = fopen("../Treatment/.car", "w");
  FILE *output = fopen("../Treatment/.out", "w");
  struct caracter *c = new_caracter();

  /* Process each detected line */
  while (l->next != NULL)
  {
    l = l->next;
    struct column *col = l->cara;
    /* Apply character-splitting algorithm */
    cutter(col, image, l->start, l->end);

    /* Calculate average character width for space detection */
    size_t count = 0;
    size_t sum_width = 0;
    struct column *scan = col->next;
    while (scan != NULL)
    {
      size_t width = scan->end - scan->start + 1;
      sum_width += width;
      count += 1;
      scan = scan->next;
    }
    size_t avg_width = count ? sum_width / count : 1;
    size_t space_threshold = avg_width / 2 + 2; /* Space if gap > avg_width/2 */

    /* Process each character in the line */
    size_t last = col->next ? col->next->start : 0;
    while (col->next != NULL)
    {
      col = col->next;
      size_t gap = col->start > last ? col->start - last : 0;
      /* Detect spaces between characters */
      if (gap > space_threshold)
        fprintf(file, "}"); /* Marker for word space */
      last = col->end;
      /* Extract 16x16 bitmap for this character */
      colcar(l->start, l->end, col, c, image);
      /* Write visual representation */
      print_car(output, c);
      /* Write binary representation for neural network */
      for (size_t i = 0; i < 256; ++i)
        fprintf(file, "%d", c->table[i]);
      fprintf(file, "\n");
      /* Reset for next character */
      caracter_free(c);
      c = new_caracter();
    }
    fprintf(file, "\t"); /* Line delimiter */
  }
  /* Clean up resources */
  caracter_free(c);
  fclose(file);
  fclose(output);
}

/*
 * newTry - Main character extraction and segmentation function
 * @string: Path to the binary (black and white) image file
 *
 * Entry point for the character extraction pipeline.
 * Loads the image, detects all lines and characters, processes each
 * character to 16x16 bitmap format, and outputs data for neural network.
 */
void newTry(char *string)
{
  SDL_Surface *image = load_image(string);
  struct line *l = new_line(0);
  /* Detect all lines and characters in the image */
  lines(image, l);
  /* Process detected structures and generate output */
  lisent(l, image);
  /* Clean up resources */
  SDL_FreeSurface(image);
  free_line(l);
}
