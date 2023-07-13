#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include<pthread.h>

#define MATRIX_SIZE 4000
#define NUM_THREADS 10

int **A;
int **B;
int **C;


void preencheMatriz(int **A) {
    int i, j;
    srand(time(NULL));
    for (i = 0; i < MATRIX_SIZE; i++) {
        for (j = 0; j < MATRIX_SIZE; j++) {
            A[i][j] = rand() % 100;
        }
    }
}

void *initialize(void *arg) {
    int i, j;
    int thread_num = *(int *)arg; // Converte o argumento passado para a thread para um inteiro

    // Cada thread inicializa algumas linhas das matrizes A e B
    for (i = thread_num; i < MATRIX_SIZE; i += NUM_THREADS) {
        for (j = 0; j < MATRIX_SIZE; j++) {
            A[i][j] = rand() % 10; // Gera um número aleatório entre 0 e 9 para cada elemento da matriz A
            B[i][j] = rand() % 10; // Gera um número aleatório entre 0 e 9 para cada elemento da matriz B
        }
    }

    return NULL;
}

void multiplicaMatrizes(int **A, int **B, int **C) {
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

void printaMatriz(int **A){
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

    pthread_t threads[NUM_THREADS]; // Declara um array de threads do tipo pthread_t
    int thread_args[NUM_THREADS]; // Declara um array de argumentos para as threads

    // Aloca memória para as matrizes A, B e C
    A = malloc(MATRIX_SIZE * sizeof(int *));
    B = malloc(MATRIX_SIZE * sizeof(int *));
    C = malloc(MATRIX_SIZE * sizeof(int *));
    for (i = 0; i < MATRIX_SIZE; i++) {
        A[i] = malloc(MATRIX_SIZE * sizeof(int));
        B[i] = malloc(MATRIX_SIZE * sizeof(int));
        C[i] = malloc(MATRIX_SIZE * sizeof(int));
    }

    for (i = 0; i < NUM_THREADS; i++) {
        thread_args[i] = i;
        pthread_create(&threads[i], NULL, initialize, &thread_args[i]);
    }

    // Espera as threads terminarem a inicialização das matrizes A e B usando pthread_join
    for (i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    /*printaMatriz(A);
    printf("\n\n");
    printaMatriz(B);*/

    multiplicaMatrizes(A, B, C);

    /*printf("\n\n");

    printaMatriz(C);*/

    struct timespec end;
    timespec_get(&end, TIME_UTC);

    double time_spent = (end.tv_sec - begin.tv_sec) + (end.tv_nsec - begin.tv_nsec) / 1000000000.0;

    printf("\n\nTime spent %lf\n", time_spent);

    // Libera a memória alocada para as matrizes A, B e C
    for (i = 0; i < MATRIX_SIZE; i++) {
        free(A[i]);
        free(B[i]);
        free(C[i]);
    }
    free(A);
    free(B);
}