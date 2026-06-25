/*
 * toBlackWhite.h
 * Image conversion module - converts color/grayscale images to binary black and white
 */

#ifndef TOBLACKWHITE_H
#define TOBLACKWHITE_H

#include <SDL/SDL.h>
#include <stdlib.h>

/*
 * toBlackWhite - Convert an image to binary black and white format
 * @file: Path to the input image file
 *
 * Description:
 *   Loads an image using SDL, applies luminance calculation with standard weights,
 *   thresholds pixels at 180, and saves the result as a BMP file.
 *   Output is saved to ../Treatment/.BaW
 */
void toBlackWhite(char *file);

#endif