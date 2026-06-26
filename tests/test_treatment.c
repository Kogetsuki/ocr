/*
 * test_treatment.c — Unit tests for the Treatment pipeline
 * Build: gcc test_treatment.c -o test_treatment -I../Treatment ../Treatment/libtreatment.a `pkg-config --cflags --libs
 * sdl` -lSDL_image
 */

#include <SDL/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "../Treatment/line_lib.h"
#include "../Treatment/pixel_operations.h"
#include "../Treatment/seg.h"
#include "../Treatment/word_lib.h"

/* ========================================================================
 * Minimal test harness
 * ======================================================================== */

static int tests_run = 0;
static int tests_passed = 0;
static int tests_failed = 0;

#define TEST(name) static void name(void)

#define RUN(name)                                                                                                      \
  do                                                                                                                   \
  {                                                                                                                    \
    tests_run++;                                                                                                       \
    printf("  %-52s", #name);                                                                                          \
    name();                                                                                                            \
    printf("PASS\n");                                                                                                  \
    tests_passed++;                                                                                                    \
  } while (0)

#define EXPECT(cond)                                                                                                   \
  do                                                                                                                   \
  {                                                                                                                    \
    if (!(cond))                                                                                                       \
    {                                                                                                                  \
      printf("FAIL\n    assertion: %s  (%s:%d)\n", #cond, __FILE__, __LINE__);                                         \
      tests_failed++;                                                                                                  \
      return;                                                                                                          \
    }                                                                                                                  \
  } while (0)

/* ========================================================================
 * Helpers
 * ======================================================================== */

/* Returns 1 if a file or directory exists at path */
static int path_exists(const char *path)
{
  struct stat st;
  return stat(path, &st) == 0;
}

/* ========================================================================
 * SDL surface / pixel tests
 * (get_pixel / put_pixel are the real pixel API from pixel_operations.h)
 * ======================================================================== */

TEST(test_sdl_init)
{
  /* SDL must initialise without error */
  int ret = SDL_Init(SDL_INIT_VIDEO);
  EXPECT(ret == 0);
  SDL_Quit();
}

TEST(test_put_get_pixel_roundtrip)
{
  SDL_Init(SDL_INIT_VIDEO);

  /* Create a tiny 4x4 surface */
  SDL_Surface *surf = SDL_CreateRGBSurface(0, 4, 4, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0);
  EXPECT(surf != NULL);

  Uint32 color = SDL_MapRGB(surf->format, 123, 45, 200);
  put_pixel(surf, 2, 2, color);
  Uint32 got = get_pixel(surf, 2, 2);

  SDL_FreeSurface(surf);
  SDL_Quit();

  EXPECT(got == color);
}

TEST(test_put_pixel_top_left_corner)
{
  SDL_Init(SDL_INIT_VIDEO);

  SDL_Surface *surf = SDL_CreateRGBSurface(0, 8, 8, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0);
  EXPECT(surf != NULL);

  Uint32 white = SDL_MapRGB(surf->format, 255, 255, 255);
  put_pixel(surf, 0, 0, white);
  Uint32 got = get_pixel(surf, 0, 0);

  SDL_FreeSurface(surf);
  SDL_Quit();

  EXPECT(got == white);
}

TEST(test_put_pixel_bottom_right_corner)
{
  SDL_Init(SDL_INIT_VIDEO);

  SDL_Surface *surf = SDL_CreateRGBSurface(0, 8, 8, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0);
  EXPECT(surf != NULL);

  Uint32 black = SDL_MapRGB(surf->format, 0, 0, 0);
  put_pixel(surf, 7, 7, black);
  Uint32 got = get_pixel(surf, 7, 7);

  SDL_FreeSurface(surf);
  SDL_Quit();

  EXPECT(got == black);
}

TEST(test_put_pixel_does_not_corrupt_neighbours)
{
  SDL_Init(SDL_INIT_VIDEO);

  SDL_Surface *surf = SDL_CreateRGBSurface(0, 4, 4, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0);
  EXPECT(surf != NULL);

  /* Fill with black, write white at (1,1), neighbours must stay black */
  Uint32 black = SDL_MapRGB(surf->format, 0, 0, 0);
  Uint32 white = SDL_MapRGB(surf->format, 255, 255, 255);

  for (int x = 0; x < 4; x++)
    for (int y = 0; y < 4; y++)
      put_pixel(surf, x, y, black);

  put_pixel(surf, 1, 1, white);

  EXPECT(get_pixel(surf, 0, 1) == black);
  EXPECT(get_pixel(surf, 2, 1) == black);
  EXPECT(get_pixel(surf, 1, 0) == black);
  EXPECT(get_pixel(surf, 1, 2) == black);

  SDL_FreeSurface(surf);
  SDL_Quit();
}

/* ========================================================================
 * word_lib / caracter struct tests
 * ======================================================================== */

TEST(test_new_caracter_not_null)
{
  struct caracter *c = new_caracter();
  EXPECT(c != NULL);
  caracter_free(c);
}

TEST(test_new_caracter_next_is_null)
{
  struct caracter *c = new_caracter();
  EXPECT(c != NULL);
  EXPECT(c->next == NULL);
  caracter_free(c);
}

TEST(test_new_caracter_table_zeroed)
{
  struct caracter *c = new_caracter();
  EXPECT(c != NULL);

  int all_zero = 1;
  for (int i = 0; i < 256; i++)
    if (c->table[i] != 0)
    {
      all_zero = 0;
      break;
    }

  caracter_free(c);
  EXPECT(all_zero);
}

TEST(test_caracter_table_size)
{
  /* table must hold exactly 256 entries (16x16) */
  struct caracter *c = new_caracter();
  EXPECT(c != NULL);

  c->table[255] = 1; /* last valid index — must not segfault */
  EXPECT(c->table[255] == 1);

  caracter_free(c);
}

TEST(test_caracter_linked_list)
{
  struct caracter *a = new_caracter();
  struct caracter *b = new_caracter();
  EXPECT(a != NULL && b != NULL);

  a->next = b;
  EXPECT(a->next == b);
  EXPECT(a->next->next == NULL);

  caracter_free(a); /* frees a and b */
}

/* ========================================================================
 * line_lib struct tests
 * ======================================================================== */

TEST(test_new_line_not_null)
{
  struct line *l = new_line(0);
  EXPECT(l != NULL);
  free_line(l);
}

TEST(test_new_line_start_set)
{
  struct line *l = new_line(42);
  EXPECT(l != NULL);
  EXPECT(l->start == 42);
  free_line(l);
}

TEST(test_new_line_next_is_null)
{
  struct line *l = new_line(0);
  EXPECT(l != NULL);
  EXPECT(l->next == NULL);
  free_line(l);
}

TEST(test_new_column_not_null)
{
  struct column *c = new_column(10);
  EXPECT(c != NULL);
  free_column(c);
}

TEST(test_new_column_start_set)
{
  struct column *c = new_column(99);
  EXPECT(c != NULL);
  EXPECT(c->start == 99);
  free_column(c);
}

TEST(test_new_column_next_null)
{
  struct column *c = new_column(0);
  EXPECT(c != NULL);
  EXPECT(c->next == NULL);
  free_column(c);
}

/* ========================================================================
 * seg() smoke tests
 * ======================================================================== */

TEST(test_seg_does_not_crash_alphabet)
{
  seg("../Treatment/tests/alphabet.png");
  EXPECT(1); /* reaching here = no crash */
}

TEST(test_seg_produces_baw_file)
{
  seg("../Treatment/tests/alphabet.png");
  EXPECT(path_exists("../Treatment/.BaW"));
}

TEST(test_seg_produces_car_file)
{
  seg("../Treatment/tests/alphabet.png");
  EXPECT(path_exists("../Treatment/.car"));
}

TEST(test_seg_does_not_crash_lorem)
{
  seg("../Treatment/tests/Lorem_2.png");
  EXPECT(1);
}

/* ========================================================================
 * Entry point
 * ======================================================================== */

int main(void)
{
  printf("\n=== Treatment Unit Tests ===\n\n");

  printf("-- SDL / pixel operations --\n");
  RUN(test_sdl_init);
  RUN(test_put_get_pixel_roundtrip);
  RUN(test_put_pixel_top_left_corner);
  RUN(test_put_pixel_bottom_right_corner);
  RUN(test_put_pixel_does_not_corrupt_neighbours);

  printf("\n-- caracter (word_lib) --\n");
  RUN(test_new_caracter_not_null);
  RUN(test_new_caracter_next_is_null);
  RUN(test_new_caracter_table_zeroed);
  RUN(test_caracter_table_size);
  RUN(test_caracter_linked_list);

  printf("\n-- line / column (line_lib) --\n");
  RUN(test_new_line_not_null);
  RUN(test_new_line_start_set);
  RUN(test_new_line_next_is_null);
  RUN(test_new_column_not_null);
  RUN(test_new_column_start_set);
  RUN(test_new_column_next_null);

  printf("\n-- seg() pipeline --\n");
  RUN(test_seg_does_not_crash_alphabet);
  RUN(test_seg_produces_baw_file);
  RUN(test_seg_produces_car_file);
  RUN(test_seg_does_not_crash_lorem);

  printf("\n============================\n");
  printf("Results: %d/%d passed", tests_passed, tests_run);
  if (tests_failed > 0)
    printf("  (%d FAILED)", tests_failed);
  printf("\n\n");

  return tests_failed > 0 ? 1 : 0;
}
