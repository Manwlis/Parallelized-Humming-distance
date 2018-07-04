/*
 ============================================================================
 Name        :	pthread_line.c
 Author      :	Manolis P, Dimitris K
 Description :	Task is set as: Compare one string from group A with all the strings from group B.
 		For each different character add 1 to the apropriate diffs[i][j]. 
 		Then each thread has to acumulate and add the lineDiff to the total diff.
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>
#include <math.h>


//	Info related to the thread's work.
struct threadInfo_str{
	int tid;
	int NumThreads;
	int lines;
	int rows;
	int lenght;
	long *totalDiff;

	//	Pointers to the arrays.
	char **StringsA;
	char **StringsB;
	int **df;
};

/*	Random string Generator	*/
void rand_str(char *dest, size_t length) {

    char charset[] = "0123456789";

    while (length-- > 0) {
        size_t index = (double) rand() / RAND_MAX * (sizeof charset - 1);
        *dest++ = charset[index];
    }
    //	*dest = '\0';	//	End of string.
}


void *compute_hamming(void *thread_info){

	//	Cast thread info.
	struct threadInfo_str* tinfo = (struct threadInfo_str*) thread_info;

	long threadSum = 0;

	//	Each threads calculates it's iterations. Ceil to better distribute the lines.
	int my_iterations = ceil((double) tinfo->lines / tinfo->NumThreads);
	int startLine  = my_iterations * tinfo->tid;
	int finishLine = my_iterations * (tinfo->tid + 1);

	/*	In case the lines/threads division is not perfect, Ceil will give the last thread
		a bigger finish line than their total number */
	if(finishLine > tinfo->lines){
		finishLine = tinfo->lines;
	}

	for(int i = startLine; i < finishLine; i++){
		for(int j = 0; j < tinfo->rows; j++){
			for(int pshfio = 0; pshfio < tinfo->lenght; pshfio++){

				if(tinfo->StringsA[i][pshfio] != tinfo->StringsB[j][pshfio])
						(tinfo->df[i][j])++;
			}
			threadSum += tinfo->df[i][j];
		}
	}

	//	Atomic actions on hardware are faster than using mutexes.
	__atomic_fetch_add(tinfo->totalDiff, threadSum, __ATOMIC_SEQ_CST);

	//	Exit. Master thread needs to return to continue the program. The rest are getting killed.
	if(tinfo->tid != 0){
		pthread_exit(NULL);
	}
	return NULL;
}


long algori8mos(int m, int n, int l, int NumThreads) {

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

	long totalDiff = 0;	//	Total hamming distance.

	//	Array with pointers to structs with info needed for the threads.
	struct threadInfo_str* thread_info[NumThreads];

	struct timeval start,finish;
	double totalTime;

	/*~~~~~~~~~~~~~~	Initialize arrays.		~~~~~~~~~~~~~~~~*/

	//	Create random strings.
	for(int i = 0; i < m; i++){
		rand_str(StringsA[i], l);
	}
	for(int i = 0; i < n; i++){
		rand_str(StringsB[i], l);
	}

	/*~~~~~~~~~~~~~~	Find hamming distances.	~~~~~~~~~~~~~~~~*/

	//	Get time of start.
 	gettimeofday(&start, NULL);

	pthread_t threads[NumThreads];
	pthread_attr_t attr;

	/* Pthreads setup: Explicitly create threads in a
     joinable state (for portability). */
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	/*	Create threads,their infos and send them to work.Do it in reverse
		succesion because the thread_info[0] will always be used last.	*/
	for(int i = NumThreads - 1; i >= 0; i--){

		//	Allocate space for thread infos.
		thread_info[i] = malloc(sizeof(struct threadInfo_str));

		//	Fill thread info.
		thread_info[i]->tid = i;
		thread_info[i]->NumThreads = NumThreads;
		thread_info[i]->lines = m;
		thread_info[i]->rows = n;
		thread_info[i]->lenght = l;
		thread_info[i]->totalDiff = &totalDiff;

		//	Connect the arrays with the threads.
		thread_info[i]->StringsA = StringsA;
		thread_info[i]->StringsB = StringsB;
		thread_info[i]->df = diffs;

		//	Create and call n-1 threads.
		if(thread_info[i]->tid != 0)
			pthread_create(&threads[i], &attr, compute_hamming, (void *) thread_info[i]);
	}

	// Send the master thread to work too.
	compute_hamming((void *) thread_info[0]);
	
	/*	Wait for the other threads to complete. */ 
	for (int i = 1; i<NumThreads; i++) {
		pthread_join(threads[i], NULL);
	}

	/*	Clean up. */
	pthread_attr_destroy(&attr);
	for(int i = 0; i < NumThreads; i++){
		free(thread_info[i]);
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
	int NumThreads = atoi(argv[4]);

	/*	Seed the pseudo-random algorithm.	*/
	srand (0);

	puts("\nTask size: 1 line.");
	printf("Total hamming distance: %ld\n", algori8mos(m, n, l, NumThreads));

	return EXIT_SUCCESS;
}
