/*
 * pixel_operations.h
 * Low-level SDL pixel manipulation and surface management utilities
 */

#ifndef PIXEL_OPERATIONS_H_
#define PIXEL_OPERATIONS_H_

#include <SDL/SDL.h>
#include <stdlib.h>

/*
 * get_pixel - Read a pixel value from an SDL surface
 * @surface: The SDL surface to read from
 * @x: X coordinate
 * @y: Y coordinate
 *
 * Return: The pixel value as a 32-bit unsigned integer
 */
Uint32 get_pixel(SDL_Surface *surface, unsigned x, unsigned y);

/*
 * put_pixel - Write a pixel value to an SDL surface
 * @surface: The SDL surface to write to
 * @x: X coordinate
 * @y: Y coordinate
 * @pixel: The pixel value to write
 */
void put_pixel(SDL_Surface *surface, unsigned x, unsigned y, Uint32 pixel);

/*
 * update_surface - Blit and update a surface on screen
 * @screen: The destination screen surface
 * @image: The source image to blit
 */
void update_surface(SDL_Surface *screen, SDL_Surface *image);

/* Initialize SDL graphics library */
void init_sdl();

/* Load an image file from disk using SDL_image */
SDL_Surface *load_image(char *path);

/* Display an image using SDL graphics */
SDL_Surface *display_image(SDL_Surface *img);

/* Wait for a keyboard press before continuing */
void wait_for_keypressed();

/* Free SDL surface memory */
void SDL_FreeSurface(SDL_Surface *surface);

#endif