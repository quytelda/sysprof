#include <math.h> //We need this to perform math


bool usegamma = false; //If we are using the gamma distribution then set this flag to true
bool usenormal = false; //If we are using the normal distribution then set this flag to true
bool useexponential = false; //If we are using the exponential distribution then set this flag to true


void gammabootstrap(float *surrogatenum[], int *surrogatefreq[], float *gammaparam[2], int numboot) //Function for calculating the population gamma distribution parameters alpha and theta given the sample distribution
{
	int samplesize = surrogatenum.length
	float mainmean = 0;   // sample mean of surrogate population distribution
	float mainvar = 0;    // sample variance of surrogate population distribution
	
	for(int i = 0; i < surrogatenum.length; i++) //calculate main mean of surrogate distribution
	{
		mainmean = mainmean + surrogatenum[i]; //add up all the samples
	}
	
	mainmean = mainmean / samplesize; //divide by the samplesize to get the mean of the sample set
	
	for(int i = 0; i < surrogatenum.length; i++) //calculate the variance of the surrogate distribution
	{
		mainvar = mainvar + ((surrogatenum[i] - mainmean)^2); //add up all the squared differences between the samples and the sample mean
	}
	
	mainvar = mainvar / samplesize; //divide by the sample size to get the variance of the sample set
	
	float mainalpha = ((mainmean)^2)/mainvar; //Find the alpha and theta parameters of the sample set. Mean of gamma distributon is equal to alpha * theta and variance of gamma distribution is equal to alpha * (theta)^2
	float maintheta = mainvar / mainmean;
	
	int bootrep = numboot; //This is the number of boot sample distributions that we will create. Each bootstrap sample distribution is built by sampling from the surrogate population with replacement
	
	float bootsamples[bootrep][samplesize]; //Create 2D array. This will hold an array of bootstrap distributions which are arrays of equal size of samples grabbed randomly from the surrogate distribution.
	float bootsampmean[bootrep]; //Create array that will hold the alpha paramater for each bootstrap gamma distribution
	float bootsampvar[bootrep]; //Create array that will hold the theta parameter for each bootstrap gamma distribution
	
	for(int i = 0; i < bootrep; i++) //Grab random samples from the surrogate gamma distribution. This sampling is done with replacement.
	{
		for(int j = 0; j < surrogatenum.length; j++)
		{
			int rondomsample = rand() % samplesize; //Randomly choose a sample
			bootrep[i][j] = randomsample; //Place sample into boostrap sample distribution. *Note: all boostrap sample sizes are equal to the surrogate sample size.
		}
	}
	
	for(int i = 0; i < bootrep; i++) //Calculate the mean for every bootstrap gamma distribution
	{
		float tempmean = 0;
		
		for(int j = 0; bootsamples[i].length; j++)
		{
			tempmean = tempmean + bootsamples[i][j]; //add up all the samples
		}
		
		tempmean = tempmean / samplesize; //divide by the samle size to get the sample distributin mean
		bootsampmean[i] = tempmean; //put the means in the array
	}
	
	for(int i = 0; i < bootrep; i++) //calculate the variance for every bootstrap gamma distribution
	{
		float tempvar = 0;
		
		for(int j = 0; j < bootsamples[i].length; j++)
		{
			tempvar = tempvar + ((bootsamples[i][j] - bootsampmean[i])^2); //add up all the squared differences of the sample and sample mean
		}
		
		tempvar = tempvar / samplesize; //divide by the sample size to get the variance
		
		bootsampvar[i] = tempvar; //put the variances in the array
	}
	
	float bootalpha[bootrep]; //create an array that will hold the alpha parameters for all the bootstrap gamma distributions
	float boottheta[bootrep]; //create an array that will hold the theta parameters for all the bootstrap gamma distributions
	
	for(int i = 0; i < bootrep; i++) //Calculate the alpha and theta parameters knowing the equations mean = alpha * theta and variance = alpha * (theta)^2
	{
		bootalpha[i] = ((bootsampmean[i])^2) / (bootsampvar[i]);
		boottheta[i] = (bootsampvar[i]) / (bootsampmean[i]);
	}
	
	float normalalphamean = 0; //Create a bell shaped normal distribution of all the calculated bootstrap alpha and theta parameters
	float normalalphavar = 0;
	
	float normalthetamean = 0;
	float normalthetavar = 0;
	
	for(int i = 0; i < bootrep; i++) //add up all the alpha and theta parameters of the boostrap gamma distributions
	{
		normalalphamean = normalalphamean + bootalpha[i];
		normalthetamean = normalthetamean + boottheta[i];
	}
	
	normalalphamean = normalalphamean / bootrep; //divide by the number of gamma distributions we made (number of alpha and theta parameters there are) to get the mean
	normalthetamean = normalthetamean / bootrep;
	
	for(int i = 0; i < bootrep; i++) //add up all the squared differences of the alpha and theta parameters minus their respective mean values
	{
		normalalphavar = normalalphavar + ((bootalpha[i] - normalalphamean)^2);
		normalthetavar = normalthetavar + ((boottheta[i] - normalthetamean)^2);
	}
	
	normalalphavar = normalalphavar / bootrep; //divide by the number of gamma distributions we made to get the variance
	normalthetavar = normalthetavar / bootrep;
	
	//the mean for the alpha and theta normal distributions is the most likely value of the actual alpha and theta parameters of the main gamma distribution of samples (surrogate distribution)
	
	float alpha = (2*mainalpha) - normalalphamean; //We know that the difference between the main sample distribution alpha and the actual population gamma distribution alpha is equal to the difference between the most likely alpha calculated by performing bootstrapping and the actual main sample distribution alpha
	float theta = (2*maintheta) - normalthetamean; //We know that the difference between the main sample distribution theta and the actual population gamma distribution theta is equal to the difference between the most likely theta calculated by performing bootstrapping and the actual main sample distribution theta
	
	gammaparam[0] = alpha;
	gammaparam[1] = theta;
}

//Please see step by step comments for gamma distribution bootstrapping funtion. This function also performs bootstrapping but on the normal distribution.
void normalbootstrap(float *surrogatenum[], int *surrogatefreq[], float normalparam[2], int numboot) //Function for calculating the population normal distribution parameters mu and sigma squared given the sample distribution
{
	int samplesize = surrogatenum.length
	float mainmean = 0;   // sample mean of surrogate population distribution
	float mainvar = 0;    // sample variance of surrogate population distribution
	
	for(int i = 0; i < surrogatenum.length; i++) //calculate main mean of surrogate distribution
	{
		mainmean = mainmean + surrogatenum[i];
	}
	
	mainmean = mainmean / samplesize;
	
	for(int i = 0; i < surrogatenum.length; i++)
	{
		mainvar = mainvar + ((surrogatenum[i] - mainmean)^2);
	}
	
	mainvar = mainvar / samplesize;
	
	int bootrep = numboot;
	
	float bootsamples[bootrep][samplesize];
	float bootsampmean[bootrep];
	float bootsampvar[bootrep];
	
	for(int i = 0; i < bootrep; i++)
	{
		for(int j = 0; j < surrogatenum.length; j++)
		{
			int rondomsample = rand() % samplesize;
			bootrep[i][j] = randomsample;
		}
	}
	
	for(int i = 0; i < bootrep; i++)
	{
		float tempmean = 0;
		
		for(int j = 0; bootsamples[i].length; j++)
		{
			tempmean = tempmean + bootsamples[i][j];
		}
		
		tempmean = tempmean / samplesize;
		bootsampmean[i] = tempmean;
	}
	
	for(int i = 0; i < bootrep; i++)
	{
		float tempvar = 0;
		
		for(int j = 0; j < bootsamples[i].length; j++)
		{
			tempvar = tempvar + ((bootsamples[i][j] - bootsampmean[i])^2);
		}
		
		tempvar = tempvar / samplesize;
		
		bootsampvar[i] = tempvar;
	}
	
	
	float normalmumean = 0;
	float normalmuvar = 0;
	
	float normalsigmamean = 0;
	float normalsigmavar = 0;
	
	for(int i = 0; i < bootrep; i++)
	{
		normalmumean = normalmumean + bootsampmean[i];
		normalsigmamean = normalsigmamean + bootsampvar[i];
	}
	
	normalmumean = normalmumean / bootrep;
	normalsigmamean = normalsigmamean / bootrep;
	
	for(int i = 0; i < bootrep; i++)
	{
		normalmuvar = normalmuvar + ((bootsampmean[i] - normalmumean)^2);
		normalsigmavar = normalsigmavar + ((bootsampvar[i] - normalsigmamean)^2);
	}
	
	normalmuvar = normalmuvar / bootrep;
	normalsigmavar = normalsigmavar / bootrep;
	
	float mu = (2*mainmean) - normalmumean;
	float sigma = (2*mainvar) - normalsigmamean;
	
	normalparam[0] = mu;
	normalparam[1] = sigma;	
}

//Please see step by step comments for gamma distribution bootstrapping funtion. This function also performs bootstrapping but on the normal distribution.
void exponentialbootstrap(float *surrogatenum[], int *surrogatefreq[], float *exponentialtheta, int numboot) //Function for calculating the population exponential distribution parameter theta given the sample distribution
{
	int samplesize = surrogatenum.length
	float mainmean = 0;   // sample mean of surrogate population distribution
	float mainvar = 0;    // sample variance of surrogate population distribution
	
	for(int i = 0; i < surrogatenum.length; i++) //calculate main mean of surrogate distribution
	{
		mainmean = mainmean + surrogatenum[i];
	}
	
	mainmean = mainmean / samplesize;
	
	for(int i = 0; i < surrogatenum.length; i++)
	{
		mainvar = mainvar + ((surrogatenum[i] - mainmean)^2);
	}
	
	mainvar = mainvar / samplesize;
	
	float maintheta = mainmean;
	
	int bootrep = numboot;
	
	float bootsamples[bootrep][samplesize];
	float bootsampmean[bootrep];
	float bootsampvar[bootrep];
	
	for(int i = 0; i < bootrep; i++)
	{
		for(int j = 0; j < surrogatenum.length; j++)
		{
			int rondomsample = rand() % samplesize;
			bootrep[i][j] = randomsample;
		}
	}
	
	for(int i = 0; i < bootrep; i++)
	{
		float tempmean = 0;
		
		for(int j = 0; bootsamples[i].length; j++)
		{
			tempmean = tempmean + bootsamples[i][j];
		}
		
		tempmean = tempmean / samplesize;
		bootsampmean[i] = tempmean;
	}
	
	for(int i = 0; i < bootrep; i++)
	{
		float tempvar = 0;
		
		for(int j = 0; j < bootsamples[i].length; j++)
		{
			tempvar = tempvar + ((bootsamples[i][j] - bootsampmean[i])^2);
		}
		
		tempvar = tempvar / samplesize;
		
		bootsampvar[i] = tempvar;
	}
	
	float boottheta[bootrep];
	
	for(int i = 0; i < bootrep; i++)
	{
		boottheta[i] = bootsampmean[i];
	}
	
	float normalthetamean = 0;
	float normalthetavar = 0;
	
	for(int i = 0; i < bootrep; i++)
	{
		normalthetamean = normalthetamean + boottheta[i];
	}
	
	normalthetamean = normalthetamean / bootrep;
	
	for(int i = 0; i < bootrep; i++)
	{
		normalthetavar = normalthetavar + ((boottheta[i] - normalthetamean)^2);
	}
	
	normalthetavar = normalthetavar / bootrep;
	
	float theta = (2*maintheta) - normalthetamean;
	
	exponential = theta;
}


float gammacutoff(float alpha, float theta, float percent) //This function determines the cutoff value for the gamma distribution for what is considered normal and what is considered a discrepancy.
{
	// The alpha and beta parameters are the gamma distribution parameters and the percent parameter is the percent of the gamma distribution that we will consider to be the cutoff value.
	percent = percent / 100.0; //percent is passed is as a number between 0.0 and 100.0 so we divide it by 100 to turn it to a number between 0.0 and 1.0
	
	int a = alpha; //letter a will be theta
	float b = theta; //letter b will be theta
	
	int faca = factorial(a); //find the factorial of alpha *Note: alpha might be a number with decimal numbers. In this case we just cut off the decimal numbers.
	float x = 0.0; //This will be out independent variable which will be returned that symbolizes the sample number at which the cutoff will be determined
	float total = 0.0; //This will be our accumulator
	
	while(total < percent) //Keep accumulating for larger values of x and once total is equal to or slightly larger than percent then we round our cutoff value
	{
		total = total + ((1/(faca * b)) * ((x / b)^(a - 1)) * exp((-1*x) / b)); //Keep adding to variable total *Note the value added here should be VERY small.
		x = x + 0.001; //Keep incrementing x. We might need to increase/decrease the increment value.
	}
	
	return x //Return our sample number cutoff value
}

int factorial(int n) //Function for calculating factorial of integer n
{
    if (n == 1)
        return 1;
    else
        return n * factorial(n - 1);
}

void main()
{
	//                            DO MySQL/SQLite things here
	
	int frequency[1440];
	float sample[1440];
	float cutoffpercent = 95.0 //Set the cutoff percentage
	float sample cutoff
	
	if(usegamma)
	{
		float gammaparameters[2];
		gammaparameters[0] = 0.0;
		gammaparameters[1] = 0.0;
		
		gammabootstrap(&sample, &frequency, &gammaparameters, 1000);
		samplecutoff = gammacutoff(gammaparameters[0], gammaparameters[1], cutoffpercent);
		
		
		// Place the new sample data and put it into the two arrays frequency[] and sample[]
		
		
	}
}


















