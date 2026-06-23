#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include "launch.h"
#include <assert.h>

void InitInput(FILE *ReadFile, double *Input, int NumInput, int WordChar)
{
  for (int i = 0; i < NumInput; i++)
  {
    Input[i] = (int)WordChar - (int)'0';
    if (i < NumInput - 1)
      WordChar = fgetc(ReadFile);
  }
}

char Transform(int k)
{
  char value = ' ';
  if (k < 26)
    value = (char)((int)'A' + k);
  else if (k >= 26 && k < 52)
    value = (char)((int)'a' + (k % 26));
  else if (k == 52)
    value = '.';
  else if (k == 53)
    value = ',';
  else if (k == 54)
    value = '?';
  else if (k == 55)
    value = '!';
  else if (k == 56)
    value = '\'';
  else if (k == 57)
    value = '(';
  else
    value = ')';
  return value;
}

char Run(double *Input, double **WeightIH, double **WeightHO, int NumInput, int NumHidden, int NumOutput)
{
  double Hidden[300];
  double Output[59];
  double HiddenSum[300];
  double OutputSum[59];

  for (int i = 0; i < NumHidden; i++)
  {
    HiddenSum[i] = WeightIH[0][i];
    for (int j = 1; j < NumInput + 1; j++)
    {
      HiddenSum[i] += Input[j - 1] * WeightIH[j][i];
    }
    Hidden[i] = 1.0 / (1.0 + exp(-HiddenSum[i]));
  }

  for (int i = 0; i < NumOutput; i++)
  {
    OutputSum[i] = WeightHO[0][i];
    for (int j = 1; j < NumHidden + 1; j++)
    {
      OutputSum[i] += Hidden[j - 1] * WeightHO[j][i];
    }
    Output[i] = 1.0 / (1.0 + exp(-OutputSum[i]));
  }

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
  // printf("%i : %lf\n", constant, constantvalue);

  return Transform(constant);
}

void FileParser1(double **WeightIH, double **WeightHO, double NumInput, double NumHidden, double NumOutput)
{
  FILE *file1 = NULL;
  file1 = fopen("test.txt", "r");
  if (!file1)
    printf("erreur");
  assert(file1);
  for (int i = 0; i < NumInput + 1; i++)
  {
    for (int j = 0; j < NumHidden; j++)
    {
      fscanf(file1, "%lf ", &WeightIH[i][j]);
    }
  }
  for (int i = 0; i < NumHidden + 1; i++)
  {
    for (int j = 0; j < NumOutput; j++)
    {
      fscanf(file1, "%lf ", &WeightHO[i][j]);
    }
  }
  fclose(file1);
}

void Parser(char *filename, double *Input, double **WeightIH, double **WeightHO, int NumInput, int NumHidden, int NumOutput)
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
  int WordChar = fgetc(ReadFile);
  while (WordChar != EOF)
  {
    if (WordChar != '\n' && WordChar != ' ')
    {
      if (WordChar == '}')
        fprintf(WriteFile, " ");
      else if (WordChar == '\t')
        fprintf(WriteFile, "\n");
      else
      {
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

void launcher(char *filename)
{
  /*------------------------ Initialise Variables ---------------------------*/

  const int NumInput = 256;
  const int NumOutput = 59;
  const int NumHidden = 300;

  /*------------------------ Allocations of memory -------------------------*/

  double *Input = malloc(NumInput * sizeof(double));

  double **WeightIH = (double **)malloc((NumInput + 1) * sizeof(double *));
  for (int i = 0; i < NumInput + 1; i++)
    WeightIH[i] = (double *)malloc(NumHidden * sizeof(double));

  double **WeightHO = (double **)malloc((NumHidden + 1) * sizeof(double *));
  for (int j = 0; j < NumHidden + 1; j++)
  {
    WeightHO[j] = (double *)malloc(NumOutput * sizeof(double));
  }

  /*------------------------- Fonctions -----------------------------------*/

  Parser(filename, Input, WeightIH, WeightHO, NumInput, NumHidden, NumOutput);

  /*------------------------- Free Memory --------------------------------*/

  for (int i = 0; i <= NumInput; i++)
    free(WeightIH[i]);
  free(WeightIH);

  for (int j = 0; j <= NumHidden; j++)
    free(WeightHO[j]);
  free(WeightHO);

  free(Input);
}
