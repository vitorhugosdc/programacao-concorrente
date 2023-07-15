#include <stdio.h> // Inclui a biblioteca padrão de entrada e saída
#include <stdlib.h> // Inclui a biblioteca padrão para funções como malloc e rand
#include <pthread.h> // Inclui a biblioteca para trabalhar com threads
#include <time.h> // Inclui a biblioteca para trabalhar com tempo

double **A; // Declara a matriz A como um ponteiro duplo para inteiros
double **B; // Declara a matriz B como um ponteiro duplo para inteiros
double **C; // Declara a matriz resultante C como um ponteiro duplo para inteiros
int MATRIX_SIZE;
int NUM_THREADS;

// Função que será executada pelas threads para inicializar as matrizes A e B
void *initialize(void *arg) { //----------------------------------------------------------------------------------------------------------------------------
    int i, j;

    int thread_num = (int)(long) arg;
    // Cada thread inicializa algumas linhas das matrizes A e B
    for (i = thread_num; i < MATRIX_SIZE; i += NUM_THREADS) {
        for (j = 0; j < MATRIX_SIZE; j++) {
            A[i][j] = (double)rand() / RAND_MAX * 10; // Gera um número aleatório entre 0 e 9 para cada elemento da matriz A
            B[i][j] = (double)rand() / RAND_MAX * 10; // Gera um número aleatório entre 0 e 9 para cada elemento da matriz B
        }
    }
    return NULL;
}

// Função que será executada pelas threads para multiplicar as matrizes A e B
void *multiply(void *arg) { //----------------------------------------------------------------------------------------------------------------------------
    int i, j, k;
    //int thread_num = *(int *)arg; // Converte o argumento passado para a thread para um inteiro

    int thread_num = (int)(long) arg;

    // Cada thread calcula algumas linhas da matriz resultante C = A * B
    for (i = thread_num; i < MATRIX_SIZE; i += NUM_THREADS) {
        for (j = 0; j < MATRIX_SIZE; j++) {
            C[i][j] = 0;
            for (k = 0; k < MATRIX_SIZE; k++) {
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

    MATRIX_SIZE = atoi(argv[1]);
    NUM_THREADS = atoi(argv[2]);

    //----------------------------------------------------------------------------------------------------------------------------

    struct timespec begin; //se p'a colocar ap'os alocaç~ao das matrizes
    timespec_get(&begin, TIME_UTC); // Obtém o tempo atual em UTC

    int i;
    pthread_t threads[NUM_THREADS]; // Declara um array de threads do tipo pthread_t
    // Aloca memória para as matrizes A, B e C usando malloc
    A = (double**)malloc(MATRIX_SIZE * sizeof(double *));
    B = (double**)malloc(MATRIX_SIZE * sizeof(double *));
    C = (double**)malloc(MATRIX_SIZE * sizeof(double *));
    for (i = 0; i < MATRIX_SIZE; i++) {
        A[i] = (double*)malloc(MATRIX_SIZE * sizeof(double));
        B[i] = (double*)malloc(MATRIX_SIZE * sizeof(double));
        C[i] = (double*)malloc(MATRIX_SIZE * sizeof(double));
    }


    //----------------------------------------------------------------------------------------------------------------------------

    srand(time(NULL)); // Inicializa a semente do gerador de números aleatórios com o tempo atual
    // Cria as threads para inicializar as matrizes A e B usando pthread_create
    for (i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, initialize, (void *)(long)i);
    }

    // Espera as threads terminarem a inicialização das matrizes A e B usando pthread_join
    for (i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // Cria as threads para multiplicar as matrizes A e B usando pthread_create
    for (i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, multiply, (void *)(long)i);
    }

    // Espera as threads terminarem a multiplicação das matrizes A e B usando pthread_join
    for (i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
     struct timespec end;
     timespec_get(&end, TIME_UTC); // Obtém o tempo atual em UTC

     double time_spent = (end.tv_sec - begin.tv_sec) + (end.tv_nsec - begin.tv_nsec) / 1000000000.0; // Calcula o tempo gasto em segundos

     printf("\n\nTime spent %lf\n", time_spent); // Imprime o tempo gasto

    // Libera a memória alocada para as matrizes A, B e C usando free
    for (i = 0; i < MATRIX_SIZE; i++) {
        free(A[i]);
        free(B[i]);
        free(C[i]);
    }
    free(A);
    free(B);
    free(C);
}
