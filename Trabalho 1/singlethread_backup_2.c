#include<stdio.h>

#include<stdlib.h>

#include<time.h>

#include<unistd.h>



#define MATRIX_SIZE 1000



void preencheMatriz(int A[MATRIX_SIZE][MATRIX_SIZE]) {

    int i, j;

    srand(time(NULL));

    for (i = 0; i < MATRIX_SIZE; i++) {

        for (j = 0; j < MATRIX_SIZE; j++) {

            A[i][j] = rand() % 100;

        }

    }

}



void multiplicaMatrizes(int A[MATRIX_SIZE][MATRIX_SIZE], int B[MATRIX_SIZE][MATRIX_SIZE], int C[MATRIX_SIZE][MATRIX_SIZE]) {

    int i, j, k;

    for (i = 0; i < MATRIX_SIZE; i++) {

        for (j = 0; j < MATRIX_SIZE; j++) {

            C[i][j] = 0;

            for (k = 0; k < MATRIX_SIZE; k++) {

                C[i][j] += A[i][k] * B[k][j];

            }

        }

    }

}



void printaMatriz(int A[MATRIX_SIZE][MATRIX_SIZE]){

    for (int i = 0; i < MATRIX_SIZE; i++) {

        for (int j = 0; j < MATRIX_SIZE; j++) {

            printf("%d ", A[i][j]);

        }

        printf("\n");

    }

}





int main(){



    struct timespec begin;

    timespec_get(&begin, TIME_UTC);



    int i, j;





    int A[MATRIX_SIZE][MATRIX_SIZE], B[MATRIX_SIZE][MATRIX_SIZE], C[MATRIX_SIZE][MATRIX_SIZE];

    

    for (i = 0; i < MATRIX_SIZE; i++) {

        for (j = 0; j < MATRIX_SIZE; j++) {

            A[i][j] = rand() % 10;

            B[i][j] = rand() % 10;

        }

    }

    printaMatriz(A);

    printf("\n\n");

    printaMatriz(B);



    multiplicaMatrizes(A, B, C);



    printf("\n\n");



    printaMatriz(C);



    struct timespec end;

    timespec_get(&end, TIME_UTC);



    double time_spent = (end.tv_sec - begin.tv_sec) + (end.tv_nsec - begin.tv_nsec) / 1000000000.0;



    printf("\n\nTime spent %lf\n", time_spent);



    return 0;

}