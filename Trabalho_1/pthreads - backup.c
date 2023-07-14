#include <stdio.h>

#include <stdlib.h>

#include <pthread.h>



#define NUM_THREADS 4 // Número de threads a serem criadas

#define MATRIX_SIZE 4 // Tamanho das matrizes



int A[MATRIX_SIZE][MATRIX_SIZE]; // Matriz A

int B[MATRIX_SIZE][MATRIX_SIZE]; // Matriz B

int C[MATRIX_SIZE][MATRIX_SIZE]; // Matriz resultante C = A * B





void imprimeMatriz(int A[MATRIX_SIZE][MATRIX_SIZE]){

    int i, j;

    printf("Matriz A E B:\n");

    for (i = 0; i < MATRIX_SIZE; i++) {

        for (j = 0; j < MATRIX_SIZE; j++) {

            printf("%d ", A[i][j]);

        }

        printf("\n");

    }

}





// Função que será executada pelas threads

void *multiply(void *arg) {

    int i, j, k;

    int thread_num = *(int *)arg; // Número da thread



    // Cada thread calcula algumas linhas da matriz resultante

    for (i = thread_num; i < MATRIX_SIZE; i += NUM_THREADS) {

        for (j = 0; j < MATRIX_SIZE; j++) {

            C[i][j] = 0;

            for (k = 0; k < MATRIX_SIZE; k++) {

                C[i][j] += A[i][k] * B[k][j];

            }

        }

    }



    return NULL;

}



int main() {

    int i, j;

    pthread_t threads[NUM_THREADS]; // Array de threads

    int thread_args[NUM_THREADS]; // Argumentos para as threads



    // Inicializa as matrizes A e B com valores aleatórios

    for (i = 0; i < MATRIX_SIZE; i++) {

        for (j = 0; j < MATRIX_SIZE; j++) {

            A[i][j] = rand() % 10;

            B[i][j] = rand() % 10;

        }

    }



    imprimeMatriz(A);

    printf("\n\n");

    imprimeMatriz(B);

    printf("\n\n");



    // Cria as threads

    for (i = 0; i < NUM_THREADS; i++) {

        thread_args[i] = i;

        pthread_create(&threads[i], NULL, multiply, &thread_args[i]); //thread_args é o void *arg da multiply

    }



    // Espera as threads terminarem

    for (i = 0; i < NUM_THREADS; i++) {

        pthread_join(threads[i], NULL);

    }



    // Imprime a matriz resultante

    printf("Matriz resultante:\n");

    for (i = 0; i < MATRIX_SIZE; i++) {

        for (j = 0; j < MATRIX_SIZE; j++) {

            printf("%d ", C[i][j]);

        }

        printf("\n");

    }



    return 0;

}