#ifndef XOR_H
#define XOR_H

void FileWriter(double **WeightIH, double **WeightHO, int NumInput, int NumHidden, int NumOutput);
void FileParser(double **WeightIH, double **WeightHO, double NumInput, double NumHidden, double NumOutput);
void DisplayWeight(double **WeightIH, double **WeightHO, int NumInput, int NumHidden, int NumOutput);
void Training(double **Input, double **Target, double **WeightIH, double **WeightHO, int NumInput, int NumHidden, int NumOutput, int NumPattern , double learningRate, double MaxError);
void RandomInit(double **WeightIH, double **WeightHO, double NumInput, double NumHidden, double NumOutput);
void InitialiseXOR(double **Input, double **Target1, int Value1, int Value2, int Target, int Target2, int Indice);
void InitialiseInput(double **Input, int NumPattern, int NumInput);
void InitialiseTarget(double **Target, int NumPattern, int NumOutput);

#endif
