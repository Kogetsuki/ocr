/*
 * OCR Image Processing Main Program
 * Converts images to black and white, then performs character segmentation
 * and extraction for optical character recognition
 */

#include <stdio.h>
#include <stdlib.h>

#include "new_try.h"
#include "toBlackWhite.h"

/*
 * main - Entry point for the image processing pipeline
 * @argc: Argument count (should be exactly 2)
 * @argv: Argument vector (argv[1] is the image file path)
 *
 * Description:
 *   Validates command line arguments, converts the input image to black and white,
 *   and then performs line/character segmentation and extraction.
 *   Output is saved to .BaW file.
 *
 * Return: 0 on success, 1 on error
 */
int main(int argc, char *argv[])
{
  /* Validate that exactly one argument (filename) is provided */
  if (argc < 2)
    errx(1, "Not enought arguments : prototype <./main `char *filename`> !");
  if (argc > 2)
    errx(1, "Too much arguments : prototype <./main `char *filename`> !");

  /* Convert input image to binary black and white */
  toBlackWhite(argv[1]);

  /* Extract lines and characters from the processed image */
  newTry(".BaW");
  return 0;
}
