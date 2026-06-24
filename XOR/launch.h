#ifndef LAUNCH_H
#define LAUNCH_H

#include <stdio.h>

void InitInput(FILE *ReadFile, double *Input, int NumInput, int WordChar);
char Transform(int k);
char Run(double *Input, double **WeightIH, double **WeightHO, int NumInput, int NumHidden, int NumOutput);
void FileParser1(double **WeightIH, double **WeightHO, double NumInput, double NumHidden, double NumOutput);
void Parser(
    char *filename, double *Input, double **WeightIH, double **WeightHO, int NumInput, int NumHidden, int NumOutput
);
void launcher(char *filename);

#endif
