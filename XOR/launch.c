/*
 * launch.c
 * Implementation of neural network inference for character recognition
 * Loads trained weights and predicts character classes from bitmaps
 */

#include "launch.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*
 * InitInput - Load a single character bitmap from file stream
 * Converts ASCII '0'/'1' to numeric 0/1 and stores in Input array
 */
void InitInput(FILE *ReadFile, double *Input, int NumInput, int WordChar)
{
  for (int i = 0; i < NumInput; i++)
  {
    /* Convert ASCII character to numeric value */
    Input[i] = (int) WordChar - (int) '0';
    if (i < NumInput - 1)
      WordChar = fgetc(ReadFile);
  }
}

/*
 * Transform - Map neural network output index to character
 * Supports uppercase letters (0-25), lowercase letters (26-51), and 8 punctuation marks (52-58)
 */
char Transform(int k)
{
  char value = ' ';

  if (k < 26)
    /* Uppercase: A-Z */
    value = (char) ((int) 'A' + k);
  else if (k >= 26 && k < 52)
    /* Lowercase: a-z */
    value = (char) ((int) 'a' + (k % 26));
  else if (k == 52)
    value = '.';
  else if (k == 53)
    value = ',';
  else if (k == 54)
    value = '?';
  else if (k == 55)
    value = '!';
  else if (k == 56)
    value = '\''; /* Single quote */
  else if (k == 57)
    value = '(';
  else
    value = ')';

  return value;
}

/*
 * Run - Forward pass through trained network for single character bitmap
 * Performs inference and returns the predicted character
 */
char Run(double *Input, double **WeightIH, double **WeightHO, int NumInput, int NumHidden, int NumOutput)
{
  /* Activation arrays for this forward pass */
  double Hidden[300];
  double Output[59];
  double HiddenSum[300]; /* Pre-activation sums */
  double OutputSum[59];

  /* FORWARD PASS: Input -> Hidden Layer */
  for (int i = 0; i < NumHidden; i++)
  {
    /* Calculate pre-activation sum with bias and weighted inputs */
    HiddenSum[i] = WeightIH[0][i]; /* Bias */
    for (int j = 1; j < NumInput + 1; j++)
      HiddenSum[i] += Input[j - 1] * WeightIH[j][i];
    /* Apply sigmoid activation */
    Hidden[i] = 1.0 / (1.0 + exp(-HiddenSum[i]));
  }

  /* FORWARD PASS: Hidden -> Output Layer */
  for (int i = 0; i < NumOutput; i++)
  {
    /* Calculate pre-activation sum with bias and weighted hidden */
    OutputSum[i] = WeightHO[0][i]; /* Bias */
    for (int j = 1; j < NumHidden + 1; j++)
      OutputSum[i] += Hidden[j - 1] * WeightHO[j][i];
    /* Apply sigmoid activation */
    Output[i] = 1.0 / (1.0 + exp(-OutputSum[i]));
  }

  /* Find output neuron with highest activation (predicted class) */
  int constant = 0;
  double constantvalue = 0;
  for (int k = 0; k < 59; k++)
  {
    if (Output[k] > constantvalue)
    {
      constant = k;
      constantvalue = Output[k];
    }
  }

  /* Convert predicted class to character */
  return Transform(constant);
}

/*
 * FileParser1 - Load trained weights from file for inference
 * Reads weights in same format as XOR.c FileWriter
 */
void FileParser1(double **WeightIH, double **WeightHO, double NumInput, double NumHidden, double NumOutput)
{
  FILE *file1 = NULL;
  file1 = fopen("../XOR/test.txt", "r");
  if (!file1)
    return;

  /* Read Input-to-Hidden weights */
  for (int i = 0; i < NumInput + 1; i++)
  {
    for (int j = 0; j < NumHidden; j++)
    {
      fscanf(file1, "%lf ", &WeightIH[i][j]);
    }
  }
  /* Read Hidden-to-Output weights */
  for (int i = 0; i < NumHidden + 1; i++)
  {
    for (int j = 0; j < NumOutput; j++)
    {
      fscanf(file1, "%lf ", &WeightHO[i][j]);
    }
  }
  fclose(file1);
}

/*
 * Parser - Main inference pipeline for all characters in a file
 * Reads character bitmaps, predicts each, and writes recognized text
 */
void Parser(
    char *filename, double *Input, double **WeightIH, double **WeightHO, int NumInput, int NumHidden, int NumOutput
)
{
  FILE *ReadFile = NULL;
  ReadFile = fopen(filename, "r");
  FILE *WriteFile = NULL;
  WriteFile = fopen("../Interface/finalresult.txt", "w+");
  char res = ' ';

  if (ReadFile == NULL)
  {
    printf("LoadFile Bug !");
    return;
  }

  /* Process file character by character */
  int WordChar = fgetc(ReadFile);
  while (WordChar != EOF)
  {
    if (WordChar != '\n' && WordChar != ' ')
    {
      if (WordChar == '}')
        /* Space delimiter */
        fprintf(WriteFile, " ");
      else if (WordChar == '\t')
        /* Line delimiter */
        fprintf(WriteFile, "\n");
      else
      {
        /* Character bitmap - perform inference */
        InitInput(ReadFile, Input, NumInput, WordChar);
        FileParser1(WeightIH, WeightHO, NumInput, NumHidden, NumOutput);
        res = Run(Input, WeightIH, WeightHO, NumInput, NumHidden, NumOutput);
        fprintf(WriteFile, "%c", res);
      }
    }
    WordChar = fgetc(ReadFile);
  }
  fclose(ReadFile);
  fclose(WriteFile);
}

/*
 * launcher - High-level entry point for character recognition
 * Allocates network, loads weights, and runs inference on file
 */
void launcher(char *filename)
{
  /* ============================================================
   * Initialize Variables
   * ============================================================ */
  const int NumInput = 256;  /* 16x16 bitmap */
  const int NumOutput = 59;  /* Character classes */
  const int NumHidden = 300; /* Hidden layer size */

  /* ============================================================
   * Allocate Memory for Neural Network
   * ============================================================ */
  /* Input vector */
  double *Input = malloc(NumInput * sizeof(double));

  /* Input-to-Hidden weight matrix */
  double **WeightIH = (double **) malloc((NumInput + 1) * sizeof(double *));
  for (int i = 0; i < NumInput + 1; i++)
    WeightIH[i] = (double *) malloc(NumHidden * sizeof(double));

  /* Hidden-to-Output weight matrix */
  double **WeightHO = (double **) malloc((NumHidden + 1) * sizeof(double *));
  for (int j = 0; j < NumHidden + 1; j++)
  {
    WeightHO[j] = (double *) malloc(NumOutput * sizeof(double));
  }

  /* ============================================================
   * Run Character Recognition
   * ============================================================ */
  Parser(filename, Input, WeightIH, WeightHO, NumInput, NumHidden, NumOutput);

  /* ============================================================
   * Free Allocated Memory
   * ============================================================ */
  for (int i = 0; i <= NumInput; i++)
    free(WeightIH[i]);
  free(WeightIH);

  for (int j = 0; j <= NumHidden; j++)
    free(WeightHO[j]);
  free(WeightHO);

  free(Input);
}
