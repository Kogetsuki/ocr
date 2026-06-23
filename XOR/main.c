#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include "XOR.h"


int main() {
    
/*------------------------ Initialise Variables ---------------------------*/

	const int NumInput = 256;
    const int NumHidden = 300;
    const int NumOutput = 59;
    const int NumPattern = 59*20;
	double MaxError = 0.0025;
    double learningRate = 0.5;
        
/*------------------------ Allocations of memory -------------------------*/

    double **Input = (double **)malloc(NumPattern* sizeof(double *));
    for(int i = 0; i < NumPattern; i++)
    	Input[i] = (double *)malloc(NumInput * sizeof(double));

    double **Target = (double **)malloc(NumPattern * sizeof(double *));
    for(int i = 0; i < NumPattern; i++)
    	Target[i] = (double *)malloc(NumOutput * sizeof(double));

    double **WeightIH = (double **)malloc((NumInput+1)* sizeof(double *));
    for(int i = 0; i< NumInput+1; i++)
    	WeightIH[i] = (double *)malloc(NumHidden * sizeof(double));

    double **WeightHO = (double **)malloc((NumHidden+1) * sizeof(double *));
    for(int j = 0; j<NumHidden+1; j++){
    	WeightHO[j] = (double *)malloc(NumOutput * sizeof(double));
    }
    
/*------------------------- Fonctions ------------------------------------*/	

// Soit on utilise RandomInit pour entrainer le réseau de neurone à partir de valeurs aléatoires
// Soit on utilise FileParser pour récupérer des valeurs de poids déja entrainées	    

	InitialiseTarget(Target, NumPattern, NumOutput);
	InitialiseInput(Input, NumPattern, NumInput);
	//FileParser(WeightIH, WeightHO, NumInput, NumHidden, NumOutput);
    RandomInit(WeightIH, WeightHO, NumInput, NumHidden, NumOutput);
    Training(Input, Target, WeightIH, WeightHO, NumInput, NumHidden, NumOutput, NumPattern, learningRate, MaxError);
    FileWriter(WeightIH, WeightHO, NumInput, NumHidden, NumOutput);

/*------------------------ Free Memory -----------------------------------*/

    for(int i = 0; i <= NumInput; i++)
    	free(WeightIH[i]);
    free(WeightIH);

    for(int j = 0; j <= NumHidden; j++)
    	free(WeightHO[j]);
    free(WeightHO);

	for(int i = 0; i < NumPattern; i++)
    	free(Input[i]);
    free(Input);

    for(int j = 0; j < NumPattern; j++)
    	free(Target[j]);
    free(Target);
}
