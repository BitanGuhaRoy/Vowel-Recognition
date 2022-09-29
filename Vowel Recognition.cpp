// Vowel Recognition.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "math.h"
#include "limits.h"
#include "float.h"
#include "conio.h"
double ci[5][51][13], ciparticular[5][11][13], ans[13], ciparticulartest[5][11][13];
double civalues[5][13];
double civaluestest[5][13];

//This function calculates Thokura's distance between two given array
double calculateThokura(double a[], double b[])
{  
	double w[] = { 1.0, 3.0, 7.0, 13.0, 19.0, 22.0, 25.0, 33.0, 42.0, 50.0, 56.0, 61.0 };
	double sum = 0.0;  //distance is initialized
	int i;
	for (i = 1; i <= 12; i++)
	{
		sum += w[i - 1] *(a[i] - b[i]) *(a[i] - b[i]); 
		
	}

	return sum; //distance is returned 
}

//This function calculates the Ci values of given vowel and record number
void findcis(char *filename, int v, int n)
{
	FILE *fp = fopen(filename, "r");

	double num;

	int b, i, j;
	for (b = 0; b < 5; b++)
	{
		double a[13][13]; //array  to calculate a values for each frame
		double k[13]; //strores Ki values for each frame
		double R[13];  //strores Ri values for each frame
		double E[13], sample[321], c[13];

		int cnt = 320, x = 0;
		double energy = 0;
		while (fscanf(fp, "%lf", &num) == 1)
		{
			//			    		printf("%lf ",num);
			if (cnt > 0)
			{
				sample[x++] = num; //Read values are stored into the array named as sample
				
				cnt--;
			}

			if (cnt == 0)	//320 samples reached let's break the current loop
			{
				break;
			}

			//		else 

		}
		//now we have a frame with 320 
		//This loop applies hamming window to the frame
		for (i = 0; i < 320; i++)
		{
			
			double tempo = (0.54 - 0.46* cos(2 * 3.14 *i / 319)); //hamming function is computed
			
			sample[i] = sample[i] *tempo;
			
		}

		for (i = 0; i <= 12; i++)
			R[i] = 0;  //R array is initialized

		//Auto correlation is computed in this nested loop
		for (i = 0; i <= 12; i++)
		{
			for (j = 0; j < 320 - i; j++)
			{
				
				R[i] += sample[j] *sample[i + j];  

			}

			
		}

		//Durbin's algorithm is applied to find the Ci values 
		E[0] = R[0];

		for (i = 1; i <= 12; i++)
		{
			double temp = 0.0;
			for (j = 1; j <= i - 1; j++)
			{
				temp += a[i - 1][j] *R[i - j];

			}

			k[i] = (R[i] - temp) / E[i - 1];

			a[i][i] = k[i];

			for (j = 1; j <= i - 1; j++)
			{
				a[i][j] = a[i - 1][j] - k[i] *a[i - 1][i - j];
			}

			E[i] = (1 - k[i] *k[i]) *E[i - 1];

			
		}

		c[0] = log(R[0] *R[0]);
		int m = 1, km = 1;
		double temp;
		for (m = 1; m <= 12; m++)
		{
			temp = 0.0;
			for (km = 1; km <= m - 1; km++)
			{
				temp += (km *c[km] *a[12][m - km]) / m;
			}

			c[m] = temp + a[12][m];
		}
		//now ci values are computed using Durbin's algorithm
		int f;
		//this funtion applies the raised sine window function on C values
		for (f = 1; f <= 12; f++)
		{
			temp = (12 / 2) *(sin((3.14 *f) / 12)); //raised sine window
			c[f] = c[f] *(1 + temp);

			ci[v][n][f] += c[f]; 
			
		}
	}

	int f;
	//this loop saves the c values of each recorded signal represented as v(Vowel index) and n recording number
	for (f = 1; f <= 12; f++)
	{
		ciparticular[v][n][f] = ci[v][n][f] / 5;//taking average of each c across 5 frames
		
	}

	

}

//This function calculates DC Value of a given file.
double dcValueCalculate(char *filename)
{
	FILE *fp = fopen(filename, "r");

	int count = 0;

	double dc = 0, num;

	while (fscanf(fp, "%lf", &num) == 1)
	{
		if (count < 100)
		{
			count++;
			dc += num; //sum of 1st 100 values in the given file.
		}
		else
			break;
	}

	dc = dc / 100; //average sample value accross 1st 100 samples is saved in dc
	return dc; //retyurn the average computed value
}

//this function finds the maximum value in a given file which will be used while normalisation
double findMax(char *filename)
{
	FILE *fp = fopen(filename, "r");

	double num, max = 0;
	while (fscanf(fp, "%lf", &num) == 1)
	{
		if (num < 0) num = num *(-1); //only positive value is taken to find the maximum value.
		if (max < num)
		{
			max = num;  //max value is updated 
		}
	}

	return max;  //max is returned 

}

//Given a file this function finds a frame which has maximum amount of energy and 
//it returns the index of the frame present at 2 index before the max energy frame
int stableFramenumber(char *fname)
{
	FILE * fp;
	fp = fopen(fname, "r");
	double amp;
	int cnt = 320;
	int i = 0;	//i denoting the number of frames
	double E = 0; //denotes energy

	double max = 0;
	int pos;	//to determine the frame with highest energy
	while (fscanf(fp, "%lf", &amp) != EOF)
	{
		if (cnt <= 320)
		{
			E = E + amp * amp;	//sum of energy of 320 samples
			cnt--;
		}

		if (cnt == 0)	//320 samples reached so energy of this frame can be calculated
		{
			E = E / (320 * 1.0);
			if (E > max)
			{
				max = E; //if max energy is updated then its position is also updated
				pos = i;
			}

			i++;
			cnt = 320;
		}
	}

	fclose(fp);
	return (pos - 2); //return the index of 2 frames before the max energy frame
}

//we read from filename1 and normalised version of the file is written in filename2
void normalisation(char *filename1, char *filename2, double max, double dc)
{
	FILE *fp1 = fopen(filename1, "r");

	FILE *fp2 = fopen(filename2, "w");

	double frac = (double) 5000 / max;  //Normalisation factor is calculated 

	int start = stableFramenumber(filename1); //stable frame number calculated

	int startind = start *320, count = 0; 

	int endind = (startind + (5 *320)); //5 steady frames are taken

	double num;

	if (max <= 5000)  //if max<=5000 then this following block is executed
	{   
		//this function is used to skip values till steady frame is found 
		while (fscanf(fp1, "%lf", &num) == 1)
		{
			if (count < startind)
			{
				count++;  
				continue;
			}
			else
				break;

		}
		//this writes steady frame's values in normalised file
		while (fscanf(fp1, "%lf", &num) == 1)
		{
			num = num - dc;


			if (count < endind)
			{
				fprintf(fp2, "%lf\n", num);

				count++;

			}
			else
				break;

		}
	}
	else  //if max<=5000 then this following block is executed
	{
		while (fscanf(fp1, "%lf", &num) == 1)
		{
			if (count < startind)
			{
				count++;
				continue;
			}

			break;

		}

		while (fscanf(fp1, "%lf", &num) == 1)
		{
			num = num - dc;

			num = num * frac;

			if (count < endind)
			{
				fprintf(fp2, "%lf\n", num);

				count++;

			}
			else

				break;

		}
	}
}

//This function is used to train and save ci values of each vowel 
void training()
{
	int i, j;
	FILE *fp, *fp1;
	int g = 0;
	char vowels[] = { 'a',
		'e',
		'i',
		'o',
		'u' };
	for (i = 0; i < 5; i++)
	{
		char filename1[30];
		char filename2[30];

		for (j = 1; j <= 10; j++)
		{
			g = 0;

			//vowels are read from the folder Data
			sprintf(filename1, "Data/224101015_%c_%d.txt", vowels[i], j);
		    //Normalised values are stored in this file
			sprintf(filename2, "224101015_%c_%d_copy.txt", vowels[i], j);

			double dc = dcValueCalculate(filename1); //dc value is calculated

			double max = findMax(filename1);

			//	normalisation(filename1,filename2,max,dc);
			FILE *fp1 = fopen(filename1, "r");

			FILE *fp2 = fopen(filename2, "w");

			double frac = (double) 5000 / max; //Normalisation fraction 

			int start = stableFramenumber(filename1);

			int startind = start *320, count = 0;

			int endind = (startind + (5 *320)); //5 steady frames are taken

			double num;

			if (max <= 5000)
			{   //this loop skips the values till the steady frames are started 
				while (fscanf(fp1, "%lf", &num) == 1)
				{
					if (count < startind)
					{
						count++;
						continue;
					}
					else
						break;

				}
				//This writes steady frame values into another file 
				while (fscanf(fp1, "%lf", &num) == 1)
				{
					num = num - dc;

					if (count < endind)
					{
						fprintf(fp2, "%lf\n", num);

						//						normalised[i][j][g++] = num;

						count++;

					}
					else
						break;

				}
			}
			else
			{   //this loop skips the values till the steady frames are started 
				while (fscanf(fp1, "%lf", &num) == 1)
				{
					if (count < startind)
					{
						count++;
						continue;
					}

					break;

				}
				//This loop writes normalised values into another file
				while (fscanf(fp1, "%lf", &num) == 1)
				{
					num = num - dc;

					num = num * frac;

					if (count < endind)
					{
						

						fprintf(fp2, "%lf\n", num);

						count++;

					}
					else

						break;

				}
			}
		}
	}

	// for each vowel and for each sample the corresponding stable frames' values are stored in normalised form 

	
	int d = 0, c = 0;
	long  double n;
	for (i = 0; i < 5; i++)
	{
		char filename3[30];
		for (j = 1; j <= 10; j++)
		{
			sprintf(filename3, "224101015_%c_%d_copy.txt", vowels[i], j);

			
			//this function calculates ci values of ith vowel's jth sample
			findcis(filename3, i, j);
		}
	}

	int q;
	double sum = 0.0;
	for (i = 0; i < 5; i++)
	{
		for (q = 1; q <= 12; q++)
		{
			for (j = 1; j <= 10; j++)
			{
				
				sum += ciparticular[i][j][q];
			}

			civalues[i][q] = sum / 10;//average is taken over 10 samples of each vowel i 
			sum = 0.0;
			
		}
	}

	for (i = 0; i < 5; i++)
	{
		for (q = 1; q <= 12; q++)
		{
			//			printf("%lf ", civalues[i][q]);
		}

		//		printf("\n");
	}

	//	double  bi=calculateThokura(ciparticular[1][1],ciparticular[1][6]);
	//		printf("%lf \n", bi);

	for (i = 0; i < 5; i++)
	{
		for (j = 1; j <= 10; j++)
		{
			for (q = 1; q <= 12; q++)
			{
							//printf("%lf ", ciparticular[i][j][q]);
			}

				//printf("\n");
		}
	}
}


int _tmain(int argc, _TCHAR* argv[])
{
		training();

		//Training is over 
		// Testing is started from this point 
	int i, j, q;
	int count=0;
	char filename3[30];
	char vowels[] = { 'a',
		'e',
		'i',
		'o',
		'u' };
	
	for (i = 0; i < 5; i++)
	{
		char filename1[30];
		char filename2[30];

		for (j = 11; j <= 20; j++)
		{
			
			//File is read from Data folder 
			sprintf(filename1, "Data/224101015_%c_%d.txt", vowels[i], j);

			sprintf(filename2, "224101015_%c_%d_test.txt", vowels[i], j);

			double dc = dcValueCalculate(filename1); //dc value is computed 

			double max = findMax(filename1); //max value is found irrespective of the sign

			//	normalisation(filename1,filename2,max,dc);
			FILE *fp1 = fopen(filename1, "r");

			FILE *fp2 = fopen(filename2, "w");

			double frac = (double) 5000 / max; //Normalize fraction is computed

			int start = stableFramenumber(filename1); //find start of steady frames 

			int startind = start *320, count = 0;

			int endind = (startind + (5 *320)); //takeen 5 steady frames

			double num;
			//Normalisation of test data is done here 
			if (max <= 5000)
			{  //values are skipped until 1st steady frame is found 
				while (fscanf(fp1, "%lf", &num) == 1)
				{
					if (count < startind)
					{
						count++;
						continue;
					}
					else
						break;

				}
				//Now steady frame starts and values are written in another file
				while (fscanf(fp1, "%lf", &num) == 1)
				{
					num = num - dc;

					if (count < endind)
					{
						fprintf(fp2, "%lf\n", num); 

						

						count++;

					}
					else
						break;

				}
			}
			else
			{    //values are skipped until 1st steady frame is found 
				while (fscanf(fp1, "%lf", &num) == 1)
				{
					if (count < startind)
					{
						count++;
						continue;
					}

					break;

				}
				//Now steady frame starts and values are written in another file
				while (fscanf(fp1, "%lf", &num) == 1)
				{
					num = num - dc;

					num = num * frac;

					if (count < endind)
					{
						

						fprintf(fp2, "%lf\n", num);

						count++;

					}
					else

						break;

				}
			}
		}
	}

	// for each vowel and for each sample the corresponding stable frames' values are stored in normalised form 

	
	int d = 0, c = 0, l, m;
	//	long double n;
	for (l = 0; l < 5; l++)
	{   count=0;
		char filename3[30];
		for (m = 11; m <= 20; m++)
		{
			sprintf(filename3, "224101015_%c_%d_test.txt", vowels[l], m);

			//			FILE *fp = fopen(filename3, "r");

			//			findcis(filename3, i, j);

			FILE *fp = fopen(filename3, "r");

			double num;

			int b, i, j;
			double citest[13];
			for(i=1;i<=12;i++) citest[i]=0; 
			for (b = 0; b < 5; b++)
			{
				double a[13][13];  //array  to calculate a values for each frame
				double k[13];   //array  to calculate K values for each frame
				double R[13];//array  to calculate R values for each frame
				double E[13], sample[321], c[13];

				int cnt = 320, x = 0;
				double energy = 0;
				
				while (fscanf(fp, "%lf", &num) == 1)
				{
					if (cnt > 0)
					{
						sample[x++] = num;  //values are stored in the sample array
						cnt--;
					}

					if (cnt == 0)	//320 samples reached so break the loop
					{
						break;
					}
				}

				for (i = 0; i < 320; i++)
				{
					double tempo = (0.54 - 0.46* cos(2 * 3.14 *i / 319));// hamming window function is applied 
					sample[i] = sample[i] *tempo;
				}

				for (i = 0; i <= 12; i++)
					R[i] = 0;

				//this below nested loop calculates the autocorrelation 
				for (i = 0; i <= 12; i++)
				{
					for (j = 0; j < 320 - i; j++)
					{
						R[i] += sample[j] *sample[i + j];  //Auto Correlation is applied 

					}
				}
				//Durbin's algorithm is applied here 
				E[0] = R[0];

				for (i = 1; i <= 12; i++)
				{
					double temp = 0.0;
					for (j = 1; j <= i - 1; j++)
					{
						temp += a[i - 1][j] *R[i - j];

					}

					k[i] = (R[i] - temp) / E[i - 1];

					a[i][i] = k[i];
					for (j = 1; j <= i - 1; j++)
					{
						a[i][j] = a[i - 1][j] - k[i] *a[i - 1][i - j];
					}

					E[i] = (1 - k[i] *k[i]) *E[i - 1];

				}

				c[0] = log(R[0] *R[0]);  //c0 value is initialized 

				int m = 1, km = 1;
				double temp;
				for (m = 1; m <= 12; m++)
				{
					temp = 0.0;
					for (km = 1; km <= m - 1; km++)
					{
						temp += (km *c[km] *a[12][m - km]) / m;
					}

					c[m] = temp + a[12][m];
				}

				int f;
				for (f = 1; f <= 12; f++)
				{
					temp = (12 / 2) *(sin((3.14 *f) / 12)); // raised sine window is applied to ci values 
					c[f] = c[f] *(1 + temp);

					citest[f] += c[f]; 
				}
			}

			int f;
			//average of cis are taken here
			for (f = 1; f <= 12; f++)
			{
				citest[f] /= 5;
					
			}

					
			//double b;
			int g, h, vpred;
			double min = DBL_MAX;  //min is initialized with max value possible
			for (g = 0; g < 5; g++)
			{
				for (h = 1; h <= 10; h++)
				{   //thokura distance is calculated and the min distance is found 
					b = calculateThokura(ciparticular[g][h], citest);
					if (b < min)
					{   //min is updated 
						min = b;
						//predicted vowel is updated
						vpred = g;
					}
				}
			}

		//	printf("%c is predicted as %c with min dist %lf \n", vowels[l], vowels[vpred], min);
		if(vowels[l]==vowels[vpred])//if original one and predicted vowel is same then increase count val.
		      count++;
		      
		
		}
	float accuracy= (count/10.0)*100.0;// accuracy for each vowel is computed 
	printf("%c prediction Accuracy is %lf%%  \n",vowels[l],accuracy);
	}
	
	getch();
	return 0;
}