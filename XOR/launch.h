/*
 * launch.h
 * Neural network prediction and character recognition module
 * Uses trained weights to classify character bitmaps
 */

#ifndef LAUNCH_H
#define LAUNCH_H

#include <stdio.h>

/*
 * InitInput - Load a single character bitmap from file
 * @ReadFile: File handle to read from
 * @Input: Array to store the 256 bitmap values
 * @NumInput: Number of input values (256)
 * @WordChar: Current character being processed
 *
 * Reads 256 binary values from file and stores in Input array
 */
void InitInput(FILE *ReadFile, double *Input, int NumInput, int WordChar);

/*
 * Transform - Convert neural network output index to character
 * @k: Output neuron index (0-58)
 *
 * Return: The corresponding character (A-Z, a-z, ., , ? ! ' ( ))
 *
 * Maps 59 output classes to printable characters and punctuation
 */
char Transform(int k);

/*
 * Run - Forward pass to predict character class
 * @Input: Input bitmap (256 values)
 * @WeightIH: Input-to-Hidden layer weights
 * @WeightHO: Hidden-to-Output layer weights
 * @NumInput: Number of input nodes (256)
 * @NumHidden: Number of hidden nodes (300)
 * @NumOutput: Number of output nodes (59)
 *
 * Return: Predicted character based on highest output activation
 *
 * Performs single forward pass through network and returns the character
 * corresponding to the neuron with highest activation
 */
char Run(double *Input, double **WeightIH, double **WeightHO, int NumInput, int NumHidden, int NumOutput);

/*
 * FileParser1 - Load trained weights from file
 * @WeightIH: Input-to-Hidden weights to fill
 * @WeightHO: Hidden-to-Output weights to fill
 * @NumInput: Number of input nodes
 * @NumHidden: Number of hidden nodes
 * @NumOutput: Number of output nodes
 *
 * Reads weights saved by XOR.c for inference/prediction
 */
void FileParser1(double **WeightIH, double **WeightHO, double NumInput, double NumHidden, double NumOutput);

/*
 * Parser - Main inference pipeline for a complete image file
 * @filename: Path to character data file
 * @Input: Input buffer
 * @WeightIH: Input-to-Hidden weights
 * @WeightHO: Hidden-to-Output weights
 * @NumInput: Number of input nodes
 * @NumHidden: Number of hidden nodes
 * @NumOutput: Number of output nodes
 *
 * Processes an entire file of character patterns and predicts each one
 */
void Parser(
    char *filename, double *Input, double **WeightIH, double **WeightHO, int NumInput, int NumHidden, int NumOutput
);

/*
 * launcher - High-level interface to run OCR prediction on a file
 * @filename: Path to the character data file
 *
 * Main entry point for character recognition:
 *   1. Allocates neural network weights
 *   2. Loads trained weights
 *   3. Processes all characters in file
 *   4. Prints predictions
 */
void launcher(char *filename);

#endif
