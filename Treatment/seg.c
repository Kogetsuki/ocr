/*
 * seg.c
 * Wrapper function combining the image segmentation pipeline
 */

#include "seg.h"

#include "new_try.h"
#include "toBlackWhite.h"

/*
 * seg - Convenience wrapper for complete image segmentation
 * @string: Path to the input image file
 *
 * Combines the two-stage segmentation process:
 *   1. Convert image to binary black and white format
 *   2. Extract and process characters for neural network input
 */
void seg(char *string)
{
  /* Stage 1: Convert to binary black and white */
  toBlackWhite(string);
  /* Stage 2: Extract lines and characters */
  newTry("../Treatment/.BaW");
}
