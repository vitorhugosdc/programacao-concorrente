#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

double **A;
double **B;
double **C;
int TAM_MATRIZ; 
int NUM_THREADS;

// Inicializa as matrizes A e B com números aleatórios de ponto flutuante entre 0.0 e 9.0
void *preencheMatriz(void *arg) {
    int i, j;

    int num_thread = (int)(long) arg;
    for (i = num_thread; i < TAM_MATRIZ; i += NUM_THREADS) {
        for (j = 0; j < TAM_MATRIZ; j++) {
            A[i][j] = (double)rand() / RAND_MAX * 10;
            B[i][j] = (double)rand() / RAND_MAX * 10;
        }
    }
    return NULL;
}

void multiplicaMatriz() {
    int i, j, k;
    for (i = 0; i < TAM_MATRIZ; i++) {
        for (j = 0; j < TAM_MATRIZ; j++) {
            C[i][j] = 0;
            for (k = 0; k < TAM_MATRIZ; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

int main(int argc, char *argv[]){

    if(argc != 3 || atoi(argv[1]) == 0 || atoi(argv[2]) == 0){
        return 0;
    }

    TAM_MATRIZ = atoi(argv[1]);
    NUM_THREADS = atoi(argv[2]);

    int i;
    pthread_t threads[NUM_THREADS];

    A = (double**)malloc(TAM_MATRIZ * sizeof(double *));
    B = (double**)malloc(TAM_MATRIZ * sizeof(double *));
    C = (double**)malloc(TAM_MATRIZ * sizeof(double *));
    for (i = 0; i < TAM_MATRIZ; i++) {
        A[i] = (double*)malloc(TAM_MATRIZ * sizeof(double));
        B[i] = (double*)malloc(TAM_MATRIZ * sizeof(double));
        C[i] = (double*)malloc(TAM_MATRIZ * sizeof(double));
    }

    srand(time(NULL));

    for (i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, &preencheMatriz, (void *)(long)i);
    }

    for (i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    struct timespec inicio;
    timespec_get(&inicio, TIME_UTC); // Obtém o tempo atual em UTC

    multiplicaMatriz();

    struct timespec fim;
    timespec_get(&fim, TIME_UTC); // Obtém o tempo atual em UTC

    double tempo_gasto = (fim.tv_sec - inicio.tv_sec) + (fim.tv_nsec - inicio.tv_nsec) / 1000000000.0;

    printf("\n\nTempo gasto: %lf\n", tempo_gasto);

    for (i = 0; i < TAM_MATRIZ; i++) {
        free(A[i]);
        free(B[i]);
        free(C[i]);
    }
    free(A);
    free(B);
    free(C);
}