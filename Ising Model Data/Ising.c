#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<math.h>
#include<string.h>

#define kB 1	//Boltzmann Constant, redefined with Coupling constants


/* Periodic Condition:
sigma[i][0]=sigma[i][N-1]
sigma[i][N]=sigma[i][1]
sigma[0][j]=sigma[0][N-1]
sigma[N][j]=sigma[1][i]

sigma[0][0]=sigma[0][N]=sigma[N][0]=sigma[N][N]=0 (futile)
 */

float* shrage_random_generator(long number, float minima, float maxima, int seed){
	int i;
	const int a = 16807;
	const int m = pow(2,31) - 1;
	const int q = floor((float)(m)/(float)(a));
	const int r = m%a;
	//printf("m=%d\nq=%d\nr=%d",m,q,r);
	float *I = (float*)malloc(number*sizeof(float));
	//srand((unsigned int)time(NULL));					//Initialize the seed
	I[0] = (float)(float)(seed);							//rand()%m
	for(i=1; i<number; i++){
		I[i] = (float)(a)*((int)(I[i-1])%q) - r*floor(I[i-1]/(float)(q));		//see 丁泽军
		if(I[i]>=0)
			continue;
		else
			I[i] = I[i] + (float)(m);
	}
	for(i=0;i<number;i++)								//Renormalize
		I[i] = I[i]/(float)(m)*(maxima-minima) + minima;
	return I;
}

void lattice_random_intialize(int **sigma, int N, int seed){		//Randomly initialize spin distribution 
	int i, j;
	float *init = NULL;
	init = shrage_random_generator(N*N, -1.0, 1.0, seed);
	for(i=0;i<N;i++){
		for(j=0;j<N;j++){
			sigma[i][j] = 0;
		}
	}
	for(i=1;i<N-1;i++){
		for(j=1;j<N-1;j++){
			if(init[j+i*N]>0){
				sigma[i][j] = +1;
				if(i==1)								//Periodic Condition
					sigma[N-1][j] = sigma[i][j];
				if(i==N-2)								//Periodic Condition
					sigma[0][j] = sigma[i][j];
				if(j==1)								//Periodic Condition
					sigma[i][N-1] = sigma[i][j];
				if(j==N-2)								//Periodic Condition
					sigma[i][0] = sigma[i][j];
			}
			else{
				sigma[i][j] = -1;
				if(i==1)								//Periodic Condition
					sigma[N-1][j] = sigma[i][j];
				if(i==N-2)								//Periodic Condition
					sigma[0][j] = sigma[i][j];
				if(j==1)								//Periodic Condition
					sigma[i][N-1] = sigma[i][j];
				if(j==N-2)								//Periodic Condition
					sigma[i][0] = sigma[i][j];
			}
		}
	}
	free(init);
}

void random_process_of_flipping(float Temperature, int **sigma, int N, int number_of_flipping_cycle, int seed){	//Evolving of spin flipping for many cycles
	int i,j,k;
	int delta_E;
	float *p=NULL;
	p = shrage_random_generator(N*N*number_of_flipping_cycle, 0.0, 1.0, seed);
	for(k=0;k<number_of_flipping_cycle;k++){			 
		for(i=1;i<N-1;i++){								//Attemp to flip by turns
			for(j=1;j<N-1;j++){
				delta_E = 2*sigma[i][j]*(sigma[i-1][j] + sigma[i+1][j] + sigma[i][j-1] + sigma[i][j+1]);
														//E_ij=2*J*sigma_i*sigma_j
														//Recall that coupling constant J is redefined with  Boltzmann constant
				
				if(delta_E<=0){							//accepted when \delta E<0 by the canonical ensemble distribution \rho=\exp(-\beta E)
					sigma[i][j] = -sigma[i][j];			//flip
					if(i==1)							//Periodic Condition
						sigma[N-1][j] = sigma[i][j];
					if(i==N-2)							//Periodic Condition
						sigma[0][j] = sigma[i][j];
					if(j==1)							//Periodic Condition
						sigma[i][N-1] = sigma[i][j];
					if(j==N-2)							//Periodic Condition
						sigma[i][0] = sigma[i][j];
				}
				else if(1.0/(1.0+exp((float)(delta_E)/(kB*Temperature)))>p[j+i*N+k*N*N]){	//Importance Sampling
						sigma[i][j] = -sigma[i][j];
						if(i==1)						//Periodic Condition
							sigma[N-1][j] = sigma[i][j];
						if(i==N-2)						//Periodic Condition
							sigma[0][j] = sigma[i][j];
						if(j==1)						//Periodic Condition
							sigma[i][N-1] = sigma[i][j];
						if(j==N-2)						//Periodic Condition
							sigma[i][0] = sigma[i][j];
					}
				else
					continue;
			}
		}
	}
	free(p);
}

void zero_initialize(int **sigma, int N){
	int i,j;
	for(i=0;i<N;i++){
		for(j=0;j<N;j++){
			sigma[i][j] = 1;	
		}
	}
}


void output(int **sigma, int N, char *order){
	int i,j;
	FILE *fp=fopen(order,"w");							//gcc: Not have to transfer char* to string for output !
	for(i=1;i<N-1;i++){
		for(j=1;j<N-1;j++){
			fprintf(fp, "%d,%d,%d\n",i,j,sigma[i][j]);		
		}
	}
}

int main(int argc, char* argv[]){						//run with external parameters
	
	int i;
	float Temperature = 3.0;
	int cycle = 2000;
	int N = 12;
	char* order;
	int seed;

	for(i=0;i<argc;i++){								//read all parameters
		if(strcmp(argv[i],"-h")==0){
			printf("\nUseage: ./Ising [Options]\nOptions:\n-h         \tList the help\n-n [number]\tSize of lattice. Default value is 10\n-c [number]\tTime for heating. Default value is 2000\n-o\tOutput order of File. No default value !");
			exit(0);
		}
		if(strcmp(argv[i],"-n")==0)						//compare two string (non-sensible on capital)
			N = atoi(argv[i+1]) + 2;					//size of lattice. Add two pair of opposite sides to make use of periordic condition 
														//atoi is a standard functino in <stdlib.h> to transform a integer char to integer
		if(strcmp(argv[i],"-c")==0){
			cycle = atoi(argv[i+1]);					//time of heating (drop configurations at early time) to gaurantee reaching the equilibrium 
		}
		if(strcmp(argv[i],"-t")==0){
			//printf("%d\n", atof(argv[i+1]));			//atof is a standard functino in <stdlib.h> to transform a float char to float
			Temperature = (float)(atof(argv[i+1]));				
		}
		if(strcmp(argv[i],"-o")==0){
			order = argv[i+1];
			seed = atoi(argv[i+1]) + 1;
		}
	}

	int **sigma = (int **)malloc(N*sizeof(int *));
	for(i=0;i<N;i++){
		sigma[i] = (int *)malloc(N*sizeof(int));
	}
	lattice_random_intialize(sigma, N, seed);
	//zero_initialize(sigma,N);
	random_process_of_flipping(Temperature, sigma, N, cycle, seed);
	output(sigma,N,order);

	return 0;
}
