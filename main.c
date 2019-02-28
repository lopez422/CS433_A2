/*
Eduardo Lopez Aguirre
Dr. Rabor
CS 433
HW2 Sudoku Tester
3/1/19

This program will test the contents of a sudoku board using a two dimessional array. The program will use a total of 27 THREADS. It will use 9 THREADS to check the rows for digits 1-9 and use 9 THREADS to check every column for digits 1-9. The rest of the 9 THREADS will be used to check the 3x3 boxes for digits 1-9. Every thread will check if the numbers in their correspondiong task is correct by sending a 1. Each thread will wait until all of them have finished and return that the solution is either vaild or invalid.

*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#define num_threads 27

/*
This will initialize the array which the threads can update to 1 if the corresponding region of the sudoku puzzle they are checking whether it is valid or invalid.
*/

int isValid[num_threads] = {0};

//this struct will define the row and column of the board.
typedef struct
{
	int row;
	int column;
} parameters;

//The board itself hard coded into the program
int my_board[9][9] = {
	{6, 2, 4, 5, 3, 9, 1, 8, 7},
	{5, 1, 9, 7, 2, 8, 6, 3, 4},
	{8, 3, 7, 6, 1, 4, 2, 9, 5},
	{1, 4, 3, 8, 6, 5, 7, 2, 9},
	{9, 5, 8, 2, 4, 7, 3, 6, 1},
	{7, 6, 2, 3, 9, 1, 4, 5, 8},
	{3, 7, 1, 9, 5, 6, 8, 4, 2},
	{4, 9, 6, 1, 8, 2, 5, 7, 3},
	{2, 8, 5, 4, 7, 3, 9, 1, 6}
};

//This function will check if the digits 1-9 appear once in a row
void *rowValid(void* param)
{
	parameters *params = (parameters*) param;
	int row = params->row;
	int col = params->column;
	if(col != 0 || row > 8)
	{
		fprintf(stderr, "Invalid row or column for row subsection! row = %d, col = %d\n", row, col);
		pthread_exit(NULL);
	}
	
	//checks if the digits 1-9 are present in the row
	int validityArray[9] = {0};
	for (int i = 0; i < 9; i++)
	{
		//if a number is encountered twice the thread will exit.
		int num = my_board[row][i];
		if (num < 1 || num > 9 || validityArray[num - 1] == 1)
			pthread_exit(NULL);
		else
			validityArray[num - 1] = 1;
	}
	
	//if everything is good to this point then row section is valid
	isValid[9 + row] = 1;
	pthread_exit(NULL);
}

//This function will chekc if the digits 1-9 appear once in a column
void *columnValid(void* param)
{
	parameters *params = (parameters*) param;
	int row = params->row;
	int col = params->column;
	if(row != 0 || col > 8)
	{
		fprintf(stderr, "Invalid row or column for column subsection! row = %d, col = %d\n", row, col);
		pthread_exit(NULL);
	}

	//checks if the digits 1-9 are only present once in the column
	int validityArray[9] = {0};
	for (int i  = 0; i < 9; i++)
	{
		int num = my_board[i][col];
		if (num < 1 ||num > 9 || validityArray[num - 1] == 1)
			pthread_exit(NULL);
		else
			validityArray[num - 1] = 1;
	}

	//if this point is reached then the column section valid
	isValid[18 + col] = 1;
	pthread_exit(NULL);
}

void *see3x3Valid(void* param)
{
	parameters *params = (parameters*) param;
	int row = params->row;
	int col = params->column;
	if(row > 6 ||row % 3 != 0 || col > 6 || col % 3 != 0)
	{
		fprintf(stderr, "Invalid row or column for subsection! row = %d, col = %d\n", row , col);
		pthread_exit(NULL);
	}
	
	int validityArray[9] = {0};
	int i, j;
	for (i = row; i < row + 3; i++)
	{
		for (j = col; j < col + 3; j++)
		{
			int num = my_board[i][j];
			if (num < 1 || num > 9|| validityArray[num -1] == 1)
				pthread_exit(NULL);
			else
				validityArray[num - 1] = 1;
		}
	}
	
	//if this point is reached then the 3x3 section is valid
	isValid[row + col/3] = 1;
	pthread_exit(NULL);
}

int main()
{
	pthread_t threads[num_threads];

	int threadI = 0;
	int i, j;

	//This will create the 9 threads for the rows and 9 for the columns and 9 more the 3x3
	//Totaling to 27 threads
	for (i = 0; i < 9; i++)
	{
		for(j = 0; j < 9; j++)
		{
			if(i%3 == 0 && j%3 == 0)
			{
				parameters *data = (parameters *) malloc(sizeof(parameters));
				data->row = i;
				data->column = j;
				pthread_create(&threads[threadI++], NULL, see3x3Valid, data);
			}

			if( i == 0)
			{
				parameters *colData = (parameters *) malloc(sizeof(parameters));
				colData->row = i;
				colData->column = j;
				pthread_create(&threads[threadI++], NULL, columnValid, colData);
			}

			if( j == 0)
			{
				parameters *rowData = (parameters *) malloc(sizeof(parameters));
				rowData->row = i;
				rowData-> column = j;
				pthread_create(&threads[threadI++], NULL, rowValid, rowData);
			}
		}
	}
	
	for(i = 0; i < num_threads; i++)
	{
		pthread_join(threads[i],NULL);
	}

	//This will see if ann of the entries in the valid array are 0, if so it will return invalid
	for (i = 0; i < num_threads; i++)
	{
		if(isValid[i] == 0)
		{
			printf("Sudoku solution is invalid!\n");
			return EXIT_SUCCESS;
		}
	}
	
	printf("Sudoku solution is Valid!!\n");
	return EXIT_SUCCESS;
}
	

