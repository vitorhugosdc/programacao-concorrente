#include<stdio.h>

#include<stdlib.h>

#include<time.h>

#include<unistd.h>



void preencheMatriz(int N, int A[N][N]) {

    int i, j;

    srand(time(NULL));

    for (i = 0; i < N; i++) {

        for (j = 0; j < N; j++) {

            A[i][j] = rand() % 100;

        }

    }

}



void multiplicaMatrizes(int N, int A[N][N], int B[N][N], int C[N][N]) {

    int i, j, k;

    for (i = 0; i < N; i++) {

        for (j = 0; j < N; j++) {

            C[i][j] = 0;

            for (k = 0; k < N; k++) {

                C[i][j] += A[i][k] * B[k][j];

            }

        }

    }

}



void printaMatriz(int N, int A[N][N]){

    for (int i = 0; i < N; i++) {

        for (int j = 0; j < N; j++) {

            printf("%d ", A[i][j]);

        }

        printf("\n");

    }

}





int main(){

    int N;

    scanf("%d", &N);

    int A[N][N], B[N][N], C[N][N];

    preencheMatriz(N, A);

    sleep(1);

    preencheMatriz(N, B);

    printaMatriz(N, A);

    printf("\n\n");

    printaMatriz(N, B);





    multiplicaMatrizes(N, A, B, C);



    printf("\n\n");



    printaMatriz(N, C);





    return 0;

}