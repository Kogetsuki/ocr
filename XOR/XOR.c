/*
 * XOR.c
 * Neural network training implementation for character recognition
 * Implements a 3-layer multi-layer perceptron (MLP) using backpropagation
 */

#include "XOR.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

/* ============================================================================
 * File I/O Operations - Save and Load Trained Weights
 * ============================================================================ */

/*
 * FileWriter - Serialize trained neural network weights to file
 * Writes all weights in sequence: Input->Hidden weights, then Hidden->Output weights
 */

void FileWriter(double **WeightIH, double **WeightHO, int NumInput, int NumHidden, int NumOutput)
{
  FILE *file1 = NULL;
  /* Open file in write mode (truncate if exists) */
  file1 = fopen("test.txt", "w+");
  /* Write Input-to-Hidden layer weights (NumInput+1 x NumHidden) */
  for (int i = 0; i < NumInput + 1; i++)
  {
    for (int j = 0; j < NumHidden; j++)
    {
      fprintf(file1, "%lf ", WeightIH[i][j]);
    }
  }

  /* Write Hidden-to-Output layer weights (NumHidden+1 x NumOutput) */
  for (int i = 0; i < NumHidden + 1; i++)
  {
    for (int j = 0; j < NumOutput; j++)
    {
      fprintf(file1, "%lf ", WeightHO[i][j]);
    }
  }

  fclose(file1);
}

/*
 * FileParser - Deserialize trained weights from file
 * Reads weights in same order as FileWriter: Input->Hidden, then Hidden->Output
 */
void FileParser(double **WeightIH, double **WeightHO, double NumInput, double NumHidden, double NumOutput)
{
  FILE *file1 = NULL;
  /* Open file in read mode */
  file1 = fopen("test.txt", "r");
  /* Read Input-to-Hidden layer weights */
  for (int i = 0; i < NumInput + 1; i++)
  {
    for (int j = 0; j < NumHidden; j++)
    {
      fscanf(file1, "%lf", &WeightIH[i][j]);
    }
  }
  /* Read Hidden-to-Output layer weights */
  for (int i = 0; i < NumHidden + 1; i++)
  {
    for (int j = 0; j < NumOutput; j++)
    {
      fscanf(file1, "%lf", &WeightHO[i][j]);
    }
  }
  fclose(file1);
}

/* ============================================================================
 * Training Algorithm - Backpropagation Neural Network Training
 * ============================================================================ */

/*
 * Training - Main training loop implementing backpropagation algorithm
 * Trains a 3-layer MLP until mean error falls below MaxError
 * Updates weights iteratively using gradient descent with specified learning rate
 */
void Training(
    double **Input,
    double **Target,
    double **WeightIH,
    double **WeightHO,
    int NumInput,
    int NumHidden,
    int NumOutput,
    int NumPattern,
    double learningRate,
    double MaxError
)
{
  /* Activation values and pre-activation sums for training set */
  double Hidden[59 * 20][300];    /* Hidden layer activations */
  double Output[59 * 20][59];     /* Output layer activations */
  double HiddenSum[59 * 20][300]; /* Hidden layer pre-activation sums (z values) */
  double OutputSum[59 * 20][59];  /* Output layer pre-activation sums */

  /* Error terms (deltas) for backpropagation */
  double d_output[59];  /* Output layer error terms */
  double d_hidden[300]; /* Hidden layer error terms */
  double d_sumOW[300];  /* Accumulated errors from output layer */

  int train = 0;             /* Iteration counter */
  double Error = NumPattern; /* Total error for current epoch */

  /* Training loop: continue until error threshold is reached */
  while ((Error / NumPattern) > MaxError)
  {
    Error = 0; /* Reset error for new epoch */
    for (int p = 0; p < NumPattern; p++)
    { /* Process each training pattern */

      /* FORWARD PASS: Input Layer -> Hidden Layer */
      for (int i = 0; i < NumHidden; i++)
      {
        /* Calculate pre-activation sum (bias + weighted input) */
        HiddenSum[p][i] = WeightIH[0][i]; /* Bias weight */
        for (int j = 1; j < NumInput + 1; j++)
        {
          HiddenSum[p][i] += Input[p][j - 1] * WeightIH[j][i];
        }
        /* Apply sigmoid activation function: 1 / (1 + e^(-z)) */
        Hidden[p][i] = 1.0 / (1.0 + exp(-HiddenSum[p][i]));
      }

      /* FORWARD PASS: Hidden Layer -> Output Layer */
      for (int i = 0; i < NumOutput; i++)
      {
        /* Calculate pre-activation sum (bias + weighted hidden) */
        OutputSum[p][i] = WeightHO[0][i]; /* Bias weight */
        for (int j = 1; j < NumHidden + 1; j++)
        {
          OutputSum[p][i] += Hidden[p][j - 1] * WeightHO[j][i];
        }
        /* Apply sigmoid activation function */
        Output[p][i] = 1.0 / (1.0 + exp(-OutputSum[p][i]));
        /* BACKPROP: Calculate output layer error term (delta) */
        /* delta_output = (target - output) * sigmoid'(z) = ... * output * (1 - output) */
        d_output[i] = (Target[p][i] - Output[p][i]) * (Output[p][i] * (1 - Output[p][i]));
      }

      /* BACKPROP: Calculate hidden layer error terms */
      for (int i = 0; i < NumHidden; i++)
      {
        /* Accumulate error from all output nodes */
        d_sumOW[i] = 0;
        for (int j = 0; j < NumOutput; j++)
        {
          d_sumOW[i] += d_output[j] * WeightHO[i + 1][j];
        }
        /* Calculate hidden layer delta: backprop * sigmoid'(z) */
        d_hidden[i] = d_sumOW[i] * (Hidden[p][i] * (1 - Hidden[p][i]));
      }

      /* WEIGHT UPDATES: Output layer weights and bias */
      for (int j = 0; j < NumOutput; j++)
      {
        /* Update bias of output neuron: bias += delta * learningRate */
        WeightHO[0][j] += d_output[j] * learningRate;
        /* Update weights: weight += hidden_activation * delta * learningRate */
        for (int i = 1; i < NumHidden + 1; i++)
        {
          WeightHO[i][j] += Hidden[p][i - 1] * d_output[j] * learningRate;
        }
      }

      /* WEIGHT UPDATES: Hidden layer weights and bias */
      for (int j = 0; j < NumHidden; j++)
      {
        /* Update bias of hidden neuron */
        WeightIH[0][j] += d_hidden[j] * learningRate;
        /* Update weights */
        for (int i = 1; i < NumInput + 1; i++)
        {
          WeightIH[i][j] += Input[p][i - 1] * d_hidden[j] * learningRate;
        }
      }

      /* Accumulate total error for this pattern */
      for (int s = 0; s < NumOutput; s++)
        Error += fabs(Target[p][s] - Output[p][s]) / NumOutput;
    }

    train++; /* Increment epoch counter */
    /* Print and save progress every 100 epochs */
    if (train % 100 == 0)
      printf("%i %lf\n", train, Error / NumPattern);
    if (train % 100 == 0)
      FileWriter(WeightIH, WeightHO, NumInput, NumHidden, NumOutput);
  }
  /* Final results */
  printf("%i %lf\n", train, Error / NumPattern);
  printf("Nombre d'itérations: %d\n", train);
}

/* ============================================================================
 * Initialization Functions - Prepare Data and Weights for Training
 * ============================================================================ */

/*
 * RandomInit - Initialize weights with small random values
 * Weights are uniformly distributed in [-0.25, 0.25] for stable training
 * Includes bias weights (index 0 for each layer)
 */
void RandomInit(double **WeightIH, double **WeightHO, double NumInput, double NumHidden, double NumOutput)
{
  /* Seed random number generator with current time */
  srand(time(NULL));

  /* Initialize Input->Hidden layer weights (including bias) */
  for (int i = 0; i <= NumInput; i++)
  {
    for (int j = 0; j < NumHidden; j++)
    {
      /* Random value in [0, 0.5] */
      WeightIH[i][j] = ((float) rand() / (float) RAND_MAX) / 2;
      /* Randomly negate for symmetric distribution */
      if (rand() % 2 < 1)
        WeightIH[i][j] *= -1;
    }
  }

  /* Initialize Hidden->Output layer weights (including bias) */
  for (int i = 0; i <= NumHidden; i++)
  {
    for (int j = 0; j < NumOutput; j++)
    {
      /* Random value in [0, 0.5] */
      WeightHO[i][j] = ((float) rand() / (float) RAND_MAX) / 2;
      /* Randomly negate for symmetric distribution */
      if (rand() % 2 < 1)
        WeightHO[i][j] *= -1;
    }
  }
}

/*
 * InitialiseTarget - Create one-hot encoded target vectors
 * Each pattern is associated with one of 59 character classes
 */
void InitialiseTarget(double **Target, int NumPattern, int NumOutput)
{
  for (int i = 0; i < NumPattern; i++)
  {
    for (int j = 0; j < NumOutput; j++)
    {
      /* One-hot encoding: target[i][i%59] = 1, all others = 0 */
      if (j == (i % 59))
        Target[i][j] = 1;
      else
        Target[i][j] = 0;
    }
  }
}

/*
 * InitialiseInput - Load character bitmaps from .car file
 * Converts ASCII '0'/'1' characters to numeric 0/1 values
 * Also skips delimiters: newlines, spaces ('}'), and tabs ('\t')
 */
void InitialiseInput(double **Input, int NumPattern, int NumInput)
{
  FILE *Dico = fopen(".car", "r");
  if (Dico == NULL)
    printf("\n\n Impossible de lire le fichier \n\n");
  int WordChar = fgetc(Dico);
  for (int i = 0; i < NumPattern; i++)
  {
    for (int j = 0; j < NumInput; j++)
    {
      /* Skip whitespace and delimiters */
      while (WordChar == '\n' || WordChar == '}' || WordChar == '\t')
        WordChar = fgetc(Dico);
      /* Convert ASCII '0'/'1' to numeric 0/1 */
      Input[i][j] = (int) WordChar - (int) '0';
      WordChar = fgetc(Dico);
    }
  }
  fclose(Dico);
}
