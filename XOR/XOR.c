#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include "XOR.h"
#include <time.h>



/*------------------------------------- File Parser -------------------------------------------------*/

void FileWriter(double **WeightIH, double **WeightHO, int NumInput, int NumHidden, int NumOutput){
	FILE* file1 = NULL;
	file1 = fopen("test.txt", "w+"); // Contenu vidé avant écriture
	for(int i = 0; i < NumInput+1; i++){
		for(int j = 0; j < NumHidden; j++){
			fprintf(file1, "%lf ", WeightIH[i][j]);
		}
	}


		for(int i = 0; i < NumHidden+1; i++){
			for(int j = 0; j < NumOutput; j++){
				fprintf(file1, "%lf ", WeightHO[i][j]);
			}
		}

		fclose(file1);
}

void FileParser(double **WeightIH, double **WeightHO, double NumInput, double NumHidden, double NumOutput)
{
    FILE* file1 = NULL;
	file1 = fopen("test.txt", "r");
    for(int i = 0; i < NumInput+1; i++){
		for(int j = 0; j < NumHidden; j++){
			fscanf(file1, "%lf", &WeightIH[i][j]);
		}
	} 
	for(int i = 0; i < NumHidden+1; i++){
		for(int j = 0; j < NumOutput; j++){
			fscanf(file1, "%lf", &WeightHO[i][j]);
		}
	}
	fclose(file1);
}

/*-------------------------------------------------------- Training -----------------------------------------------------*/

void Training(double **Input, double **Target, double **WeightIH, double **WeightHO, int NumInput, int NumHidden, 
													int NumOutput, int NumPattern , double learningRate, double MaxError){
	double Hidden[59*20][300];	// NumPattern, NumHidden
	double Output[59*20][59];    //NumPattern, NumOutput
	double HiddenSum[59*20][300];  //NumPattern, NumHidden

	double OutputSum[59*20][59];  //NumPattern, NumOutput

    double d_output[59];  // NumOutput
    double d_hidden[300];	//NumHidden

    double d_sumOW[300]; //NumHidden
    int train = 0;

    double Error = NumPattern;
	while((Error/NumPattern)>MaxError) {    //(Error/NumPattern)>0.1
		Error = 0;
        for(int p = 0; p < NumPattern; p++){

	        for (int i = 0; i < NumHidden; i++) {  //Forward I -> H

	        	HiddenSum[p][i] = WeightIH[0][i];
	        	for(int j = 1; j < NumInput+1; j++){
	        		HiddenSum[p][i] += Input[p][j-1] * WeightIH[j][i];
	        	}
	        	Hidden[p][i] = 1.0 / (1.0 + exp(-HiddenSum[p][i]));
	        }
			

	        for(int i = 0; i < NumOutput; i++){		//Forward H -> O
	        	OutputSum[p][i] = WeightHO[0][i];
	        	for(int j = 1; j < NumHidden+1; j++){
	        		OutputSum[p][i]+= Hidden[p][j-1] * WeightHO[j][i];
	        	}
	        	Output[p][i] = 1.0 / (1.0 + exp(-OutputSum[p][i]));    // DeltaOutput ci-dessous
	        	d_output[i] = (Target[p][i] - Output[p][i]) * (Output[p][i] * (1 - Output[p][i]));
	        }

	        for(int i = 0; i < NumHidden; i++){
	        	d_sumOW[i] = 0;
	        	for(int j = 0; j < NumOutput; j++){
	        		d_sumOW[i] += d_output[j] * WeightHO[i+1][j];
	        	}
	        	d_hidden[i] = d_sumOW[i] * (Hidden[p][i] * (1 - Hidden[p][i])); // Calcul of DeltaHidden
	        }

	        for(int j=0; j < NumOutput; j++){
	        	WeightHO[0][j] += d_output[j] * learningRate; // Update Bias of Output
	        	for(int i = 1; i < NumHidden+1; i++){
	        		WeightHO[i][j] += Hidden[p][i-1] * d_output[j] * learningRate; //Update Weight of Output
	        	}

	        }
			

	        for(int j =0; j < NumHidden; j++){
	        	WeightIH[0][j] += d_hidden[j] * learningRate;   // Update Bias of Hidden
	        	for(int i =1; i < NumInput+1; i++){
	        		WeightIH[i][j] += Input[p][i-1] * d_hidden[j] * learningRate;  //Update Weight of Hidden
	        	}
	        }
            for(int s = 0; s < NumOutput; s++)
	        	Error += fabs(Target[p][s] - Output[p][s])/NumOutput;
		}

		train+=1;
		if (train %100 == 0)
			printf("%i %lf\n", train, Error/NumPattern);
	    if (train%100 == 0)
			FileWriter(WeightIH, WeightHO, NumInput, NumHidden, NumOutput);
	}
	printf("%i %lf\n", train, Error/NumPattern);
    printf("Nombre d'itérations: %d\n", train);
}


/*----------------------------------------Initialise Matrix---------------------------------------------------*/

void RandomInit(double **WeightIH, double **WeightHO, double NumInput, double NumHidden, double NumOutput){
	srand(time(NULL));
	for (int i =0; i <= NumInput; i++){  //Initialisation
	        	for(int j = 0; j < NumHidden; j++){
	        		WeightIH[i][j] = ((float)rand() / (float)RAND_MAX)/2;
					if (rand()%2 < 1)
						WeightIH[i][j] *= -1;
	        	}
	}
	for(int i = 0; i <= NumHidden; i++){
		for(int j = 0; j < NumOutput; j++){
			WeightHO[i][j] = ((float)rand() / (float)RAND_MAX)/2;
			if(rand()%2 < 1)
				WeightHO[i][j] *= -1;
		}
	}

}

void InitialiseTarget(double **Target, int NumPattern, int NumOutput){
	for(int i = 0; i < NumPattern; i++)
	{
		for(int j = 0; j < NumOutput; j++)
		{
			if (j == (i%59))
				Target[i][j] = 1;
			else 
				Target[i][j] = 0;
		}
	}
}

void InitialiseInput(double **Input, int NumPattern, int NumInput){
	FILE *Dico = fopen(".car", "r");
	if (Dico== NULL)
		printf("\n\n Impossible de lire le fichier \n\n");
	int WordChar = fgetc(Dico);
	for(int i = 0; i < NumPattern; i++)
	{
		for(int j = 0; j < NumInput; j++)
		{
			while(WordChar == '\n' || WordChar == '}'|| WordChar == '\t')			
				WordChar = fgetc(Dico);
			Input[i][j] = (int)WordChar - (int)'0';
			WordChar = fgetc(Dico);
		}
	}
	fclose(Dico);
}
