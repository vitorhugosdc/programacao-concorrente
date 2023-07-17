#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

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

// Função que será executada pelas threads para multiplicar as matrizes A e B
void *multiplicaMatriz(void *arg) { //----------------------------------------------------------------------------------------------------------------------------
    int i, j, k;
    int num_thread = (int)(long) arg; //mesma ideia da função initialize

    // Cada thread calcula algumas linhas da matriz resultante C = A * B
    for (i = num_thread; i < TAM_MATRIZ; i += NUM_THREADS) { //mesma ideia da função initialize
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

    if(argc != 3 || atoi(argv[1]) == 0 || atoi(argv[2]) == 0){ //verifica se o número de threads ou o tamanho da matriz são maiores que 0
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
        //o primeiro argumento é o ponteiro para a variável que armazenará o identificador da thread criada
        //o terceiro argumento é um ponteiro pra função que será executada pelas threads
        //o quarto argumento é um ponteiro para o argumento que será passado para a função, ou seja, o valor de i é convertido para um ponteiro void * (pq é preciso a conversão), 
        //esse ponteiro que vai ser usado como argumento recebido pelas funções (ele é convertido pra long antes pra evitar aviso de compilação))
    }

    // Espera as threads terminarem a inicialização das matrizes A e B usando pthread_join
    for (i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    //----------------------------------------------------------------------------------------------------------------------------

    struct timespec inicio;
    timespec_get(&inicio, TIME_UTC); // Obtém o tempo atual em UTC

    // Cria as threads para multiplicar as matrizes A e B usando pthread_create
    for (i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, &multiplicaMatriz, (void *)(long)i); //mesma ideia
    }

    // Espera as threads terminarem a multiplicação das matrizes A e B usando pthread_join
    for (i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    //----------------------------------------------------------------------------------------------------------------------------
    
    struct timespec fim;
    timespec_get(&fim, TIME_UTC); // Obtém o tempo atual em UTC

    double tempo_gasto = (fim.tv_sec - inicio.tv_sec) + (fim.tv_nsec - inicio.tv_nsec) / 1000000000.0; // Calcula o tempo gasto em segundos

    printf("\n\nTempo gasto: %lf\n", tempo_gasto); // Imprime o tempo gasto

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
