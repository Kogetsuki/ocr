/*
 * seg.h
 * Wrapper function for complete image segmentation pipeline
 */

#ifndef SEG_H
#define SEG_H

/*
 * seg - Wrapper function combining black/white conversion and character extraction
 * @string: Path to input image file
 *
 * Description:
 *   Convenience function that performs the complete segmentation pipeline:
 *   converts to black and white, then extracts and processes characters.
 */
void seg(char *string);

#endif
