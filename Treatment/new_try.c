#include "line_lib.h"
#include "new_try.h"
#include "word_lib.h"
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "pixel_operations.h"
#include <stdlib.h>

void columns(SDL_Surface* image, struct line *line, size_t width) {
    int ended = 1;
    struct column *curr = line->cara;
    for (size_t i = 0; i < width; ++i)
    {
        Uint8 color = 1;
        for (size_t j = line->start; color != 0 && j < line->end; ++j)
        {
            SDL_GetRGB(get_pixel(image, i, j),
                image->format, &color, &color, &color);
        }
        if (color == 0 && ended)
        {
            curr->next = new_column(i);
            curr = curr->next;
            ended = 0;
        }
        else if (color != 0 && !ended)
        {
            curr->end = i-1;
            ended = 1;
        }
    }
    if (!ended) curr->end = width-1;
}

void lines(char *string, struct line *l) {
    SDL_Surface* image = load_image(string);
    size_t height = image->h;
    size_t width = image->w;
    int ended = 1;
    struct line *curr = l;
    for (size_t i = 0; i < height; ++i)
    {
        Uint8 color = 1;
        for (size_t j = 0; color != 0 && j < width; ++j)
        {
            SDL_GetRGB(get_pixel(image, j, i),
                image->format, &color, &color, &color);
        }
        if (color == 0 && ended)
        {
            curr->next = new_line(i);
            curr = curr ->next;
            ended = 0;
        }
        else if (color != 0 && !ended)
        {
            curr->end = i-1;
            ended = 1;
            columns(image, curr, width);
        }
    }
    if (!ended)
    {
        curr->end = height-1;
        columns(image, curr, width);
    }
    SDL_FreeSurface(image);
}

void colcar(size_t y1, size_t y2, struct column *col, struct caracter *car,
        char *string) {
    SDL_Surface* image = load_image(string);
    size_t max = y2-y1 < col->end-col->start ? col->end+1-col->start : y2+1-y1;
    size_t xstart = (max-col->end+col->start)/2;
    size_t ystart = (max-y2+y1)/2;
    float passage = 16./(float)max;
    for (size_t x = col->start; x <= col->end; ++x)
    {
        for (size_t y = y1; y <= y2; ++y)
        {
            Uint8 color;
            SDL_GetRGB(get_pixel(image, x, y),
                image->format, &color, &color, &color);
            if (color == 0)
            {
                size_t x1car = passage*(x-col->start+xstart);
                size_t x2car = passage*(x+1-col->start+xstart);
                size_t y1car = passage*(y-y1+ystart);
                size_t y2car = passage*(y+1-y1+ystart);
                for (size_t xcar = x1car; xcar < x2car; ++xcar)
                    for (size_t ycar = y1car; ycar < y2car; ++ycar)
                        car->table[xcar+ycar*16] = 1;
            }
        }
    }
    SDL_FreeSurface(image);
}

void print_car(FILE *file, struct caracter *c) {
    for (size_t i = 0; i < 16; ++i) {
        for (size_t j = 0; j < 16; ++j) {
	    fprintf(file, c->table[i*16+j] ? "x" : " ");
	}
	fprintf(file, "\n");
    }
    fprintf(file, "\n\n");
}

void lisent(char *string, struct line *l) {
    FILE *file = fopen("../Treatment/.car", "w");
    FILE *output = fopen("../Treatment/.out", "w");
    struct caracter *c = new_caracter();
    while (l->next != NULL) {
        l = l->next;
        struct column *col = l->cara;
        cutter(col);
	size_t last = col->next->start;
        while (col->next != NULL) {
            col = col->next;
	    if (col->end-col->start <= (col->start-last)) fprintf(file, "}");
	    last = col->end;
            colcar(l->start, l->end, col, c, string);
	    print_car(output, c);
	    for (size_t i = 0; i < 256; ++i) fprintf(file, "%d", c->table[i]);
	    fprintf(file, "\n");
	    caracter_free(c);
	    c = new_caracter();
        }
	fprintf(file, "\t");
    }
    caracter_free(c);
    fclose(file);
    fclose(output);
}

void newTry(char *string) {
    struct line *l = new_line(0);
    lines(string, l);
    lisent(string, l);
    free_line(l);
}
