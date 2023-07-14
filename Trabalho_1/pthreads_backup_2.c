#include <stdio.h>

#include <stdlib.h>

#include <pthread.h>

#include <time.h>



#define NUM_THREADS 10 // Número de threads a serem criadas

#define MATRIX_SIZE 15000// Tamanho das matrizes



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



    struct timespec begin;

    timespec_get(&begin, TIME_UTC);



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



    /*imprimeMatriz(A);

    printf("\n\n");

    imprimeMatriz(B);

    printf("\n\n");*/



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



    struct timespec end;

    timespec_get(&end, TIME_UTC);



    double time_spent = (end.tv_sec - begin.tv_sec) + (end.tv_nsec - begin.tv_nsec) / 1000000000.0;



    printf("\n\nTime spent %lf\n", time_spent);



    return 0;

}