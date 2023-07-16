#include <stdio.h> // Inclui a biblioteca padrão de entrada e saída
#include <stdlib.h> // Inclui a biblioteca padrão para funções como malloc e rand
#include <pthread.h> // Inclui a biblioteca para trabalhar com threads
#include <time.h> // Inclui a biblioteca para trabalhar com tempo

double **A; // Declara a matriz A como um ponteiro duplo para inteiros
double **B; // Declara a matriz B como um ponteiro duplo para inteiros
double **C; // Declara a matriz resultante C como um ponteiro duplo para inteiros
int TAM_MATRIZ;
int NUM_THREADS;

// Função que será executada pelas threads para inicializar as matrizes A e B
void *preencheMatriz(void *arg) { //----------------------------------------------------------------------------------------------------------------------------
    int i, j;

    int num_thread = (int)(long) arg;
    // Cada thread inicializa algumas linhas das matrizes A e B
    for (i = num_thread; i < TAM_MATRIZ; i += NUM_THREADS) {
        for (j = 0; j < TAM_MATRIZ; j++) {
            A[i][j] = (double)rand() / RAND_MAX * 10; // Gera um número aleatório entre 0 e 9 para cada elemento da matriz A
            B[i][j] = (double)rand() / RAND_MAX * 10; // Gera um número aleatório entre 0 e 9 para cada elemento da matriz B
        }
    }
    return NULL;
}

// Função que será executada pelas threads para multiplicar as matrizes A e B
void *multiplicaMatriz(void *arg) { //----------------------------------------------------------------------------------------------------------------------------
    int i, j, k;
    int num_thread = (int)(long) arg;
    // Cada thread calcula algumas linhas da matriz resultante C = A * B
    for (i = num_thread; i < TAM_MATRIZ; i += NUM_THREADS) {
        for (j = 0; j < TAM_MATRIZ; j++) {
            C[i][j] = 0;
            for (k = 0; k < TAM_MATRIZ; k++) {
                C[i][j] += A[i][k] * B[k][j]; // Calcula o valor de cada elemento da matriz resultante C
            }
        }
    }
    return NULL;
}

int main(int argc, char *argv[]) {

    if(argc != 3 || atoi(argv[1]) == 0 || atoi(argv[2]) == 0){
        return 0;
    }

    TAM_MATRIZ = atoi(argv[1]);
    NUM_THREADS = atoi(argv[2]);

    //----------------------------------------------------------------------------------------------------------------------------

    int i;
    pthread_t threads[NUM_THREADS]; // Declara um array de threads do tipo pthread_t
    // Aloca memória para as matrizes A, B e C usando malloc
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

    struct timespec inicio; //se p'a colocar ap'os alocaç~ao das matrizes
    timespec_get(&inicio, TIME_UTC); // Obtém o tempo atual em UTC

    // Cria as threads para multiplicar as matrizes A e B usando pthread_create
    for (i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, &multiplicaMatriz, (void *)(long)i);
    }

    // Espera as threads terminarem a multiplicação das matrizes A e B usando pthread_join
    for (i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
     struct timespec fim;
     timespec_get(&fim, TIME_UTC); // Obtém o tempo atual em UTC

     double tempo_gasto = (fim.tv_sec - inicio.tv_sec) + (fim.tv_nsec - inicio.tv_nsec) / 1000000000.0; // Calcula o tempo gasto em segundos

     printf("\n\nTempo gasto: %lf\n", tempo_gasto); // Imprime o tempo gasto

    // Libera a memória alocada para as matrizes A, B e C usando free
    for (i = 0; i < TAM_MATRIZ; i++) {
        free(A[i]);
        free(B[i]);
        free(C[i]);
    }
    free(A);
    free(B);
    free(C);
}
