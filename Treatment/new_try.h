/*
 * new_try.h
 * Character segmentation and extraction from processed images
 */

#ifndef NEW_TRY_H
#define NEW_TRY_H

#include <SDL.h>
#include <stdlib.h>

/*
 * newTry - Extract lines and characters from a binary image
 * @string: Path to the input binary image file
 *
 * Description:
 *   Main entry point for character extraction pipeline.
 *   Detects lines and character boundaries, applies character resizing to 16x16,
 *   and outputs results to .car and .out files for neural network processing.
 */
void newTry(char *string);

#endif
