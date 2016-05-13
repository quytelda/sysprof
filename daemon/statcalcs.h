#ifndef STATCALCS_HEADER_FILE
#define STATCALCS_HEADER_FILE

#include <stdbool.h>

bool usegamma = false; //If we are using the gamma distribution then set this flag to true
bool usenormal = false; //If we are using the normal distribution then set this flag to true
bool useexponential = false; //If we are using the exponential distribution then set this flag to true

void gammabootstrap(float *surrogatenum[], int *surrogatefreq[], float *gammaparam[2], int numboot, int surrogatesize); //Function for calculating the population gamma distribution parameters alpha and theta given the sample distribution

void normalbootstrap(float *surrogatenum[], int *surrogatefreq[], float *normalparam[2], int numboot, int surrogatesize);

void exponentialbootstrap(float *surrogatenum[], int *surrogatefreq[], float *exponentialtheta, int numboot, int surrogatesize);

int factorial(int n); //Function for calculating factorial of integer n

float gammacutoff(float alpha, float theta, float percent);

float exponentialcutoff(float theta, float percent);

float normalcutoff(float mu, float sigma, float percent);

int main();

#endif
