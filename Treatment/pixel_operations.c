/*
 * pixel_operations.c
 * Low-level pixel manipulation utilities using SDL library
 * Provides generic pixel read/write operations for various color depths
 */

#include "pixel_operations.h"

#include <stdio.h>
#include <stdlib.h>

#include "SDL/SDL.h"
#include "SDL/SDL_image.h"

/*
 * pixel_ref - Get raw pointer to a pixel at given coordinates
 * @surf: The SDL surface
 * @x: X coordinate
 * @y: Y coordinate
 *
 * Return: Pointer to the pixel data
 *
 * Note: This is a low-level utility function that calculates the memory
 * address of a pixel based on surface pitch and bytes per pixel.
 */
static inline Uint8 *pixel_ref(SDL_Surface *surf, unsigned x, unsigned y)
{
  int bpp = surf->format->BytesPerPixel;
  return (Uint8 *) surf->pixels + y * surf->pitch + x * bpp;
}

/*
 * get_pixel - Read a pixel value from the surface
 * @surface: The SDL surface to read from
 * @x: X coordinate
 * @y: Y coordinate
 *
 * Return: 32-bit pixel value (supports 1, 2, 3, and 4 byte color depths)
 *
 * Handles different color depths (8-bit, 16-bit, 24-bit, 32-bit) and
 * endianness concerns for proper pixel value retrieval.
 */
Uint32 get_pixel(SDL_Surface *surface, unsigned x, unsigned y)
{
  Uint8 *p = pixel_ref(surface, x, y);

  switch (surface->format->BytesPerPixel)
  {
    case 1: /* 8-bit per pixel */
      return *p;

    case 2: /* 16-bit per pixel */
      return *(Uint16 *) p;

    case 3: /* 24-bit per pixel - handle endianness */
      if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
        return p[0] << 16 | p[1] << 8 | p[2];
      else
        return p[0] | p[1] << 8 | p[2] << 16;

    case 4: /* 32-bit per pixel */
      return *(Uint32 *) p;
  }

  return 0;
}

/*
 * put_pixel - Write a pixel value to the surface
 * @surface: The SDL surface to write to
 * @x: X coordinate
 * @y: Y coordinate
 * @pixel: The 32-bit pixel value to write
 *
 * Handles different color depths (8-bit, 16-bit, 24-bit, 32-bit) and
 * endianness concerns for proper pixel value writing.
 */
void put_pixel(SDL_Surface *surface, unsigned x, unsigned y, Uint32 pixel)
{
  Uint8 *p = pixel_ref(surface, x, y);

  switch (surface->format->BytesPerPixel)
  {
    case 1: /* 8-bit per pixel */
      *p = pixel;
      break;

    case 2: /* 16-bit per pixel */
      *(Uint16 *) p = pixel;
      break;

    case 3: /* 24-bit per pixel - handle endianness */
      if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
      {
        p[0] = (pixel >> 16) & 0xff;
        p[1] = (pixel >> 8) & 0xff;
        p[2] = pixel & 0xff;
      }
      else
      {
        p[0] = pixel & 0xff;
        p[1] = (pixel >> 8) & 0xff;
        p[2] = (pixel >> 16) & 0xff;
      }
      break;

    case 4: /* 32-bit per pixel */
      *(Uint32 *) p = pixel;
      break;
  }
}

/*
 * update_surface - Blit modified image to screen and update display
 * @screen: The screen surface to update
 * @image: The source image to blit
 *
 * Copies the modified image to the screen buffer and updates the
 * specified region to make changes visible.
 */
void update_surface(SDL_Surface *screen, SDL_Surface *image)
{
  if (SDL_BlitSurface(image, NULL, screen, NULL) < 0)
    fprintf(stderr, "BlitSurface error: %s\n", SDL_GetError());

  SDL_UpdateRect(screen, 0, 0, image->w, image->h);
}

void init_sdl()
{
  // Init only the video part.
  // If it fails, die with an error message.
  if (SDL_Init(SDL_INIT_VIDEO) == -1)
  {
    fprintf(stderr, "Could not initialize SDL: %s.\n", SDL_GetError());
    exit(EXIT_FAILURE);
  }
}

SDL_Surface *load_image(char *path)
{
  SDL_Surface *img;

  // Load an image using SDL_image with format detection.
  // If it fails, die with an error message.
  img = IMG_Load(path);
  if (!img)
  {
    fprintf(stderr, "can't load %s: %s\n", path, IMG_GetError());
    exit(EXIT_FAILURE);
  }

  return img;
}

SDL_Surface *display_image(SDL_Surface *img)
{
  SDL_Surface *screen;

  // Set the window to the same size as the image
  screen = SDL_SetVideoMode(img->w, img->h, 0, SDL_SWSURFACE | SDL_ANYFORMAT);
  if (screen == NULL)
  {
    // error management
    fprintf(stderr, "Couldn't set %dx%d video mode: %s\n", img->w, img->h, SDL_GetError());
    exit(EXIT_FAILURE);
  }

  // Blit onto the screen surface
  if (SDL_BlitSurface(img, NULL, screen, NULL) < 0)
  {
    fprintf(stderr, "BlitSurface error: %s\n", SDL_GetError());
    exit(EXIT_FAILURE);
  }

  // Update the screen
  SDL_UpdateRect(screen, 0, 0, img->w, img->h);

  // return the screen for further uses
  return screen;
}

void wait_for_keypressed()
{
  SDL_Event event;

  // Wait for a key to be down.
  do
  {
    SDL_PollEvent(&event);
  } while (event.type != SDL_KEYDOWN);

  // Wait for a key to be up.
  do
  {
    SDL_PollEvent(&event);
  } while (event.type != SDL_KEYUP);
}

void SDL_FreeSurface(SDL_Surface *surface);