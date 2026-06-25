/*
 * toBlackWhite.c
 * Image conversion module - implements binary black and white image conversion
 * using standard luminance formula and thresholding
 */

#include "toBlackWhite.h"

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>

#include "pixel_operations.h"

/*
 * toBlackWhite - Convert a color or grayscale image to binary black and white
 * @file: Path to the input image file
 *
 * Loads an image, converts each pixel to grayscale using the standard
 * luminance formula (0.3*R + 0.59*G + 0.11*B), applies a threshold at 180,
 * and saves the binary result as a BMP file for further processing.
 * Output is saved to ../Treatment/.BaW
 */
void toBlackWhite(char *file)
{
  SDL_Surface *image_surface;
  init_sdl();
  image_surface = load_image(file);
  int width = image_surface->w;
  int height = image_surface->h;

  /* Process each pixel in the image */
  for (int x = 0; x < width; ++x)
  {
    for (int y = 0; y < height; ++y)
    {
      Uint8 r, g, b;
      /* Extract RGB values from the current pixel */
      SDL_GetRGB(get_pixel(image_surface, x, y), image_surface->format, &r, &g, &b);
      /* Apply luminance formula and threshold: > 180 -> white, else -> black */
      r = g = b = (0.3 * r + 0.59 * g + 0.11 * b) > 180 ? 255 : 0;
      /* Write the binary pixel back to the surface */
      put_pixel(image_surface, x, y, SDL_MapRGB(image_surface->format, r, g, b));
    }
  }
  /* Save the processed image as BMP file */
  SDL_SaveBMP(image_surface, "../Treatment/.BaW");
  SDL_FreeSurface(image_surface);
}
