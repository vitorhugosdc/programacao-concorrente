#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

double **A;
double **B;
double **C;
int MATRIX_SIZE; 
int NUM_THREADS;

// Função que será executada pelas threads para inicializar as matrizes A e B
void *initialize(void *arg) {
    int i, j;

    int thread_num = (int)(long) arg;

    printf("\n pthread_args: %d\n\n",thread_num);

    // Cada thread inicializa algumas linhas das matrizes A e B
    for (i = thread_num; i < MATRIX_SIZE; i += NUM_THREADS) {
        for (j = 0; j < MATRIX_SIZE; j++) {
            A[i][j] = rand() % 10; // Gera um número aleatório entre 0 e 9 para cada elemento da matriz A
            B[i][j] = rand() % 10; // Gera um número aleatório entre 0 e 9 para cada elemento da matriz B
        }
    }
    return NULL;
}

void multiplicaMatrizes(double **A, double **B, double **C) {
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

void printaMatriz(double **A){
    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            printf("%.10f ", A[i][j]);
        }
        printf("\n");
    }
}

int main(int argc, char *argv[]){

    if(argc != 3 || atoi(argv[1]) == 0 || atoi(argv[2]) == 0){
        return 0;
    }

    MATRIX_SIZE = atoi(argv[1]);
    NUM_THREADS = atoi(argv[2]);

    struct timespec begin;
    timespec_get(&begin, TIME_UTC);

    int i;

    pthread_t threads[NUM_THREADS]; // Declara um array de threads do tipo pthread_t


    // Aloca memória para as matrizes A, B e C
    A = (double**)malloc(MATRIX_SIZE * sizeof(double *));
    B = (double**)malloc(MATRIX_SIZE * sizeof(double *));
    C = (double**)malloc(MATRIX_SIZE * sizeof(double *));
    for (i = 0; i < MATRIX_SIZE; i++) {
        A[i] = (double*)malloc(MATRIX_SIZE * sizeof(double));
        B[i] = (double*)malloc(MATRIX_SIZE * sizeof(double));
        C[i] = (double*)malloc(MATRIX_SIZE * sizeof(double));
    }

    srand(time(NULL)); // Inicializa a semente do gerador de números aleatórios com o tempo atual
    // Cria as threads para inicializar as matrizes A e B usando pthread_create
    for (i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, initialize, (void *)(long)i);
    }

    // Espera as threads terminarem a inicialização das matrizes A e B usando pthread_join
    for (i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    multiplicaMatrizes(A, B, C); //n precisa passar como parametro

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
    free(C);
}