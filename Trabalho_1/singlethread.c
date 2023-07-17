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
void *preencheMatriz(void *arg) { //----------------------------------------------------------------------------------------------------------------------------
    int i, j;

    int num_thread = (int)(long) arg; //é o "ID" da thread, serve pra calcular quais linhas da matriz a thread vai calcular
    /*
        Por exemplo: supondo que estamos utilizando 10 threads, a thread 0 executará as linhas 0, 10, 20,... (número da thread + 10)
                     a thread 1 executará as linhas 1, 11, 21, ... (número da thread + 10)                
    */

    // Cada thread inicializa algumas linhas das matrizes A e B
    for (i = num_thread; i < TAM_MATRIZ; i += NUM_THREADS) { //(número da thread + 10 para 10 threads)
        for (j = 0; j < TAM_MATRIZ; j++) {
            A[i][j] = (double)rand() / RAND_MAX * 10;
            B[i][j] = (double)rand() / RAND_MAX * 10;
        }
    }
    return NULL;
}

//multiplicação de matrizes padrão SINGLETHREAD
void multiplicaMatriz() { //----------------------------------------------------------------------------------------------------------------------------
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

    if(argc != 3 || atoi(argv[1]) == 0 || atoi(argv[2]) == 0){ //verifica se o número de threads ou o tamanho da matriz são maiores que 0
        return 0;
    }

    TAM_MATRIZ = atoi(argv[1]);
    NUM_THREADS = atoi(argv[2]); //as threads nesse caso só são usadas para inicialização das matrizes, os cálculos de multiplicação são todos singlethread

    int i;
    pthread_t threads[NUM_THREADS]; // Declara um array de threads do tipo pthread_t

    // Aloca memória para as matrizes A, B e C
    A = (double**)malloc(TAM_MATRIZ * sizeof(double *));
    B = (double**)malloc(TAM_MATRIZ * sizeof(double *));
    C = (double**)malloc(TAM_MATRIZ * sizeof(double *));
    for (i = 0; i < TAM_MATRIZ; i++) {
        A[i] = (double*)malloc(TAM_MATRIZ * sizeof(double));
        B[i] = (double*)malloc(TAM_MATRIZ * sizeof(double));
        C[i] = (double*)malloc(TAM_MATRIZ * sizeof(double));
    }

    //----------------------------------------------------------------------------------------------------------------------------

    srand(time(NULL)); // Inicializa a semente do gerador de números aleatórios com o tempo atual

    // Cria as threads para inicializar as matrizes A e B usando pthread_create
    for (i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, &preencheMatriz, (void *)(long)i);
    }

    // Espera as threads terminarem a inicialização das matrizes A e B usando pthread_join
    for (i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    //----------------------------------------------------------------------------------------------------------------------------

    struct timespec inicio;
    timespec_get(&inicio, TIME_UTC); // Obtém o tempo atual em UTC

    multiplicaMatriz();

    struct timespec fim;
    timespec_get(&fim, TIME_UTC); // Obtém o tempo atual em UTC

    double tempo_gasto = (fim.tv_sec - inicio.tv_sec) + (fim.tv_nsec - inicio.tv_nsec) / 1000000000.0;

    printf("\n\nTempo gasto: %lf\n", tempo_gasto);

    // Libera a memória alocada para as matrizes A, B e C
    for (i = 0; i < TAM_MATRIZ; i++) {
        free(A[i]);
        free(B[i]);
        free(C[i]);
    }
    free(A);
    free(B);
    free(C);
}