#include <err.h>
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "pixel_operations.h"
#include "toBlackWhite.h"

void toBlackWhite(char *file) {
    SDL_Surface* image_surface;
    init_sdl();
    image_surface = load_image(file);
    int width = image_surface->w;
    int height = image_surface->h;

    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            Uint8 r,g,b;
            SDL_GetRGB(get_pixel(image_surface, x, y),
                image_surface->format, &r, &g, &b);
            r = g = b = (0.3*r + 0.59*g + 0.11*b) > 180 ? 255 : 0;
            put_pixel(image_surface, x, y,
                SDL_MapRGB(image_surface->format, r, g, b));
        }
    }
    SDL_SaveBMP(image_surface, "../Treatment/.BaW");
    SDL_FreeSurface(image_surface);
}
