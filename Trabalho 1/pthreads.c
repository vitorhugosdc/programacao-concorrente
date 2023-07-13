#include <stdio.h> // Inclui a biblioteca padrão de entrada e saída
#include <stdlib.h> // Inclui a biblioteca padrão para funções como malloc e rand
#include <pthread.h> // Inclui a biblioteca para trabalhar com threads
#include <time.h> // Inclui a biblioteca para trabalhar com tempo

#define NUM_THREADS 10 // Define o número de threads a serem criadas
#define MATRIX_SIZE 4000 // Define o tamanho das matrizes

int **A; // Declara a matriz A como um ponteiro duplo para inteiros
int **B; // Declara a matriz B como um ponteiro duplo para inteiros
int **C; // Declara a matriz resultante C como um ponteiro duplo para inteiros

// Função que imprime uma matriz passada como argumento
void imprimeMatriz(int **A){
    int i, j;
    printf("Matriz A E B:\n");
    for (i = 0; i < MATRIX_SIZE; i++) {
        for (j = 0; j < MATRIX_SIZE; j++) {
            printf("%d ", A[i][j]); // Imprime cada elemento da matriz
        }
        printf("\n"); // Pula uma linha após imprimir cada linha da matriz
    }
}

// Função que será executada pelas threads para inicializar as matrizes A e B
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

// Função que será executada pelas threads para multiplicar as matrizes A e B
void *multiply(void *arg) {
    int i, j, k;
    int thread_num = *(int *)arg; // Converte o argumento passado para a thread para um inteiro

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

int main() { // Função principal do programa

    struct timespec begin; //se p'a colocar ap'os alocaç~ao das matrizes
    timespec_get(&begin, TIME_UTC); // Obtém o tempo atual em UTC

    int i;
    pthread_t threads[NUM_THREADS]; // Declara um array de threads do tipo pthread_t
    int thread_args[NUM_THREADS]; // Declara um array de argumentos para as threads

    // Aloca memória para as matrizes A, B e C usando malloc
    A = malloc(MATRIX_SIZE * sizeof(int *));
    B = malloc(MATRIX_SIZE * sizeof(int *));
    C = malloc(MATRIX_SIZE * sizeof(int *));
    for (i = 0; i < MATRIX_SIZE; i++) {
        A[i] = malloc(MATRIX_SIZE * sizeof(int));
        B[i] = malloc(MATRIX_SIZE * sizeof(int));
        C[i] = malloc(MATRIX_SIZE * sizeof(int));
    }

    srand(time(NULL)); // Inicializa a semente do gerador de números aleatórios com o tempo atual

    // Cria as threads para inicializar as matrizes A e B usando pthread_create
    for (i = 0; i < NUM_THREADS; i++) {
        thread_args[i] = i;
        pthread_create(&threads[i], NULL, initialize, &thread_args[i]);
    }

    // Espera as threads terminarem a inicialização das matrizes A e B usando pthread_join
    for (i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    /*imprimeMatriz(A);
    printf("\n\n");
    imprimeMatriz(B);
    printf("\n\n");*/

    // Cria as threads para multiplicar as matrizes A e B usando pthread_create
    for (i = 0; i < NUM_THREADS; i++) {
        thread_args[i] = i;
        pthread_create(&threads[i], NULL, multiply, &thread_args[i]);
    }

    // Espera as threads terminarem a multiplicação das matrizes A e B usando pthread_join
    for (i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

   /* printf("Matriz resultante:\n");
     for (i = 0; i < MATRIX_SIZE; i++) {
         for (j = 0; j < MATRIX_SIZE; j++) {
             printf("%d ", C[i][j]); // Imprime cada elemento da matriz resultante C
         }
         printf("\n"); // Pula uma linha após imprimir cada linha da matriz
     }*/

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
}
