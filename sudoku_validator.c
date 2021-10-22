/* 
 * Name: Salmon, LaSean
 * Email: lsalmo1@lsu.edu
 * Project: PA-1 (Multithreading)
 * Instructor: Feng Chen
 * Class: cs4103-sp21
 * Login ID: cs410397
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

pthread_spinlock_t lock;
int pshared;
int ret;

const int Rows = 9, Cols = 9;
int Valid = 1;
int sol[81];

struct parameters {
	int row;
	int col;
};

void copyArray (int a[], int b[]) {
	for(int i=0; i<(81); i++) {
		a[i] = b[i];
	}
}

void *checkRows (void *arg1){

	struct parameters *assign = (struct parameters*) arg1;
	int solution[Rows][Cols];
	int testRow = assign->row;
	free(arg1);
	int temp[81];
	copyArray(temp, sol);

	for(int i = 0; i < Rows; i++) {
		for(int j = 0; j < Cols; j++) {			
			solution[i][j] = temp[i*Rows + j];
		}
	}

	int checkVals[Cols];
	for (int j = 0; j < Cols; j++) {
		int num = solution[testRow][j];

		if (num%1!=0 || num<1 || num>9 || checkVals[num-1]==1) {
			ret = pthread_spin_lock(&lock);
			Valid = 0;
			ret = pthread_spin_unlock(&lock);
			printf("Thread 1: Row %d is invalid.\n", (testRow+1));
			pthread_exit(NULL);
		}
		else { checkVals[num-1] = 1; }
	}

	printf("Thread 1: Row %d is valid.\n", (testRow+1));
	pthread_exit(NULL);

}

void *checkColumns (void *arg2) {
	
	struct parameters *assign = (struct parameters*) arg2;	
	int solution[Rows][Cols];
	int testCol = assign->col;
	free(arg2);
	int temp[81];
	copyArray(temp, sol);

	for(int i = 0; i < Rows; i++) {
		for(int j = 0; j < Cols; j++) {			
			solution[i][j] = temp[i*Rows + j];
		}
	} 

	int checkVals[Rows];
	for (int j = 0; j < Rows; j++) {
		int num = solution[j][testCol];

		if(num%1!=0 || num<1 || num>9 || checkVals[num-1]==1) {
			ret = pthread_spin_lock(&lock);
			Valid = 0;
			ret = pthread_spin_unlock(&lock);
			printf("Thread 2: Column %d is invalid.\n", (testCol+1));
			pthread_exit(NULL);
		}
		else { checkVals[num-1] = 1; }
	}

	printf("Thread 2: Column %d is valid.\n", (testCol+1));
	pthread_exit(NULL);
}

void *checkGrids (void *arg3) {
	
	struct parameters *assign = (struct parameters*) arg3;
	int gridVals[Rows];
	int row = assign->row;
	int col = assign->col;
	int solution[81];
	copyArray(solution, sol);

	for (int i=0; i<3; i++) {
		gridVals[i] = solution[row*9 + col+i];
	}
	for (int i=3; i<6; i++) {
		gridVals[i] = solution[(row*9)+9 + col+(i-3)];
	}
	for (int i=6; i<9; i++) {
		gridVals[i] = solution[(row*9)+18 + col+(i-6)];
	}

	int checkVals[Rows];
	for (int i=0; i<Rows; i++) {
		if(gridVals[i]%1!=0 || gridVals[i]<1 || gridVals[i]>9 || checkVals[gridVals[i]-1]==1) {
			ret = pthread_spin_lock(&lock);
			Valid = 0;
			ret = pthread_spin_unlock(&lock);
			printf("Thread 3: Grid [%d][%d] is invalid.\n", (row+1), (col+1));
			pthread_exit(NULL);
		}
		else { checkVals[gridVals[i]-1] = 1; }
	}

	
	printf("Thread 3: Grid [%d][%d] is valid.\n", (row/3+1), (col/3+1));
	pthread_exit(NULL);

}

int main() {

	FILE *filePointer = fopen("solution.txt", "r");
	int num = 0, scanNum = 0;

	if (filePointer != NULL) {
		
		while (fscanf(filePointer, "%d", &num) != EOF) {
			
			sol[scanNum] = num;
			scanNum++;
		}
	}
	else { printf("File does not exist. \n"); }
	fclose(filePointer);


	ret = pthread_spin_init(&lock, pshared);
	int threadTotal = 27;
	int threadCt = 0;
	pthread_t threadID[threadTotal];

	for (int i=0; i<9; i++) {
		struct parameters *arg1;
		arg1 = malloc(sizeof(struct parameters));
		arg1->row=i;
		pthread_create(&threadID[threadCt++], NULL, checkRows, (void*) arg1);

		struct parameters *arg2;
		arg2 = malloc(sizeof(struct parameters));
		arg2->col=i;
		pthread_create(&threadID[threadCt++], NULL, checkColumns, (void*) arg2);

		if (i<3) {
			for(int j=0; j<3; j++) {
				struct parameters *arg3;
				arg3 = malloc(sizeof(struct parameters));
				arg3->row=i*3;
				arg3->col=j*3;
				pthread_create(&threadID[threadCt++], NULL, checkGrids, (void*) arg3);
			}
		}
	}

	for (int i=0; i<threadTotal; i++) {
		pthread_join(threadID[i], NULL);
	}

	printf("\n");

	if (Valid == 0) {
		printf("The solution is invalid.\n");
	}
	else { printf("The solution is valid!\n"); }

	return 0;
}


