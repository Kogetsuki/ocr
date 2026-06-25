/*
 * XOR Neural Network Training Main Program
 * Trains a multi-layer perceptron on character recognition task
 * Uses backpropagation algorithm to learn weight matrices
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "XOR.h"

int main()
{
  /* ============================================================================
   * Initialize Training Parameters and Network Architecture
   * ============================================================================ */

  const int NumInput = 256;       /* Input layer: 16x16 character bitmap */
  const int NumHidden = 300;      /* Hidden layer nodes */
  const int NumOutput = 59;       /* Output layer: 59 character classes */
  const int NumPattern = 59 * 20; /* Training set size (1180 patterns) */
  double MaxError = 0.0025;       /* Target error threshold for convergence */
  double learningRate = 0.5;      /* Learning rate for backpropagation */

  /* ============================================================================
   * Allocate Memory for Training Data
   * ============================================================================ */

  /* Input patterns matrix: NumPattern x NumInput */
  double **Input = (double **) malloc(NumPattern * sizeof(double *));
  for (int i = 0; i < NumPattern; i++)
    Input[i] = (double *) malloc(NumInput * sizeof(double));

  /* Target vectors matrix: NumPattern x NumOutput */
  double **Target = (double **) malloc(NumPattern * sizeof(double *));
  for (int i = 0; i < NumPattern; i++)
    Target[i] = (double *) malloc(NumOutput * sizeof(double));

  /* Input-to-Hidden weight matrix (NumInput+1 x NumHidden) */
  /* +1 accounts for bias weights */
  double **WeightIH = (double **) malloc((NumInput + 1) * sizeof(double *));
  for (int i = 0; i < NumInput + 1; i++)
    WeightIH[i] = (double *) malloc(NumHidden * sizeof(double));

  /* Hidden-to-Output weight matrix (NumHidden+1 x NumOutput) */
  double **WeightHO = (double **) malloc((NumHidden + 1) * sizeof(double *));
  for (int j = 0; j < NumHidden + 1; j++)
  {
    WeightHO[j] = (double *) malloc(NumOutput * sizeof(double));
  }

  /* ============================================================================
   * Initialize Training Data and Weights
   * ============================================================================ */

  /* Two options:
   * 1. FileParser - Load previously trained weights and continue training
   * 2. RandomInit - Start with random weights from scratch
   * Choose the appropriate option based on your needs.
   */

  /* Initialize target vectors with one-hot encoding */
  InitialiseTarget(Target, NumPattern, NumOutput);
  /* Load training input patterns from .car file */
  InitialiseInput(Input, NumPattern, NumInput);
  /* Option 1: Load previously trained weights (commented out) */
  /* FileParser(WeightIH, WeightHO, NumInput, NumHidden, NumOutput); */
  /* Option 2: Initialize with random weights */
  RandomInit(WeightIH, WeightHO, NumInput, NumHidden, NumOutput);

  /* Start training process */
  Training(Input, Target, WeightIH, WeightHO, NumInput, NumHidden, NumOutput, NumPattern, learningRate, MaxError);

  /* Save trained weights for later use */
  FileWriter(WeightIH, WeightHO, NumInput, NumHidden, NumOutput);

  /* ============================================================================
   * Free Allocated Memory
   * ============================================================================ */

  /* Free Input-to-Hidden weights */
  for (int i = 0; i <= NumInput; i++)
    free(WeightIH[i]);
  free(WeightIH);

  /* Free Hidden-to-Output weights */
  for (int j = 0; j <= NumHidden; j++)
    free(WeightHO[j]);
  free(WeightHO);

  /* Free input patterns */
  for (int i = 0; i < NumPattern; i++)
    free(Input[i]);
  free(Input);

  /* Free target vectors */
  for (int j = 0; j < NumPattern; j++)
    free(Target[j]);
  free(Target);

  return 0;
}
