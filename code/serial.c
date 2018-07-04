/*
 ============================================================================
 Name        : serial.c
 Author      : Manolis P, Dimitris K
 Description : Serial algorithm.
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>


/*	Random string Generator	*/
void rand_str(char *dest, size_t length) {

    char charset[] = "0123456789";

    while (length-- > 0) {
        size_t index = (double) rand() / RAND_MAX * (sizeof charset - 1);
        *dest++ = charset[index];
    }
    
    //	Need in order to display string arrays.
    //	*dest = '\0';	//	End of string.
}


long algori8mos(int m, int n, int l) {

	//	Array to store distances. Preset all values to 0.
	int** diffs = (int**) malloc( m * sizeof(int*));
	for (int i = 0; i < m; i++)
	   diffs[i] = (int*) calloc(l, n * sizeof(int));

	//	Arrays for the random strings.
	char** StringsA = (char**) malloc( m * sizeof(char*));
	for (int i = 0; i < m; i++)
	   StringsA[i] = (char*) malloc( l * sizeof(char));

	char** StringsB = (char**) malloc( n * sizeof(char*));
	for (int i = 0; i < n; i++)
	   StringsB[i] = (char*) malloc( l * sizeof(char));

	//	Total hamming distance.
	long totalDiff = 0;	

	struct timeval start,finish;
	double totalTime;

	/*~~~~~~~~~~~~~~~	Initialize arrays.		~~~~~~~~~~~~~~~~*/

	//	Create random strings.
	for(int i = 0; i < m; i++){
		rand_str(StringsA[i], l);
	}
	for(int i = 0; i < n; i++){
		rand_str(StringsB[i], l);
	}

	/*~~~~~~~~~~~~~~~	Find hamming distances.	~~~~~~~~~~~~~~~~*/

	//	Get time of start.
 	gettimeofday(&start, NULL);

	for(int i = 0; i < m; i++){
		for(int j = 0; j < n; j++){
			for(int pshfio = 0; pshfio < l; pshfio++){

				//	For every different character, increase pair's distance by 1.
				if(StringsA[i][pshfio] != StringsB[j][pshfio]){

					diffs[i][j]++;
				}
			}
			//	Calculate total distance.
			totalDiff += diffs[i][j];
		}
	}

	//	Get time of finish.
	gettimeofday(&finish, NULL);
	
	/*~~~~~~~~~~~~~~~	Free memory. Finish.	~~~~~~~~~~~~~~~~*/

	free(diffs);
	free(StringsA);
	free(StringsB);

	//	Calculate and print total time.
	totalTime = 
	(double)(finish.tv_usec - start.tv_usec) / 1000.0L + 
	(double)(finish.tv_sec - start.tv_sec) * 1000.0L;
	printf("Total time: %f ms\n", totalTime);

	return totalDiff;
}


int main(int argc, const char** argv) {

	int m = atoi(argv[1]);
	int n = atoi(argv[2]);
	int l = atoi(argv[3]);

	/*	Seed the pseudo-random algorithm.	*/
	srand (0);

	puts("\nSerial algorithm starts.");
	printf("Total humming distance: %ld\n", algori8mos(m, n, l));

	return EXIT_SUCCESS;
}
