/**
 * This version is stamped on May 10, 2016
 *
 * Contact:
 *   Louis-Noel Pouchet <pouchet.ohio-state.edu>
 *   Tomofumi Yuki <tomofumi.yuki.fr>
 *
 * Web address: http://polybench.sourceforge.net
 */
/* trisolv.c: this file is part of PolyBench/C */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <pthread.h>

/* Include polybench common header. */
#include <polybench.h>

/* Include benchmark-specific header. */
#include "trisolv.h"

#define NUM_THREADS 4

typedef struct {
    int thread_id;
    int start_row;
    int end_row;
    DATA_TYPE **L;
    DATA_TYPE *x;
    DATA_TYPE *b;
} thread_data;

pthread_barrier_t barrier;

void *parallel_forward_substitution(void *arg) {
    thread_data *data = (thread_data *) arg;
    int start_row = data->start_row;
    int end_row = data->end_row;
    DATA_TYPE L = data->L;
    DATA_TYPE *x = data->x;
    DATA_TYPE *b = data->b;

    for (int i = start_row; i <= end_row; i++) {
        x[i] = b[i];
        for (int j = 0; j < i; j++) {
            x[i] -= **L[i][j] * x[j];
        }
        x[i] /= (*L)[i][i];
        pthread_barrier_wait(&barrier);
    }

    pthread_exit(NULL);
}

/* Array initialization. */
static
void init_array(int n,
                DATA_TYPE POLYBENCH_2D(L,N,N,n,n),
                DATA_TYPE POLYBENCH_1D(x,N,n),
                DATA_TYPE POLYBENCH_1D(b,N,n))
{
  int i, j;

  for (i = 0; i < n; i++)
    {
      x[i] = - 999;
      b[i] =  i ;
      for (j = 0; j <= i; j++)
        L[i][j] = (DATA_TYPE) (i+n-j+1)*2/n;
    }
}


/* DCE code. Must scan the entire live-out data.
   Can be used also to check the correctness of the output. */
static
void print_array(int n,
                 DATA_TYPE POLYBENCH_1D(x,N,n))

{
  int i;

  POLYBENCH_DUMP_START;
  POLYBENCH_DUMP_BEGIN("x");
  for (i = 0; i < n; i++) {
    fprintf (POLYBENCH_DUMP_TARGET, DATA_PRINTF_MODIFIER, x[i]);
    if (i % 20 == 0) fprintf (POLYBENCH_DUMP_TARGET, "\n");
  }
  POLYBENCH_DUMP_END("x");
  POLYBENCH_DUMP_FINISH;
}


/* Main computational kernel. The whole function will be timed,
   including the call and return. */
static
void kernel_trisolv_parallel(int n,
                             DATA_TYPE POLYBENCH_2D(L,N,N,n,n),
                             DATA_TYPE POLYBENCH_1D(x,N,n),
                             DATA_TYPE POLYBENCH_1D(b,N,n))
{
  pthread_t threads[NUM_THREADS];
  thread_data data[NUM_THREADS];

  pthread_barrier_init(&barrier, NULL, NUM_THREADS);

  for (int i = 0; i < NUM_THREADS; i++) {
      data[i].thread_id = i;
      data[i].start_row = i * (n / NUM_THREADS);
      data[i].end_row = (i + 1) * (n / NUM_THREADS) - 1;
      data[i].L = L;
      data[i].x = x;
      data[i].b = b;
      pthread_create(&threads[i], NULL, parallel_forward_substitution, &data[i]);
  }

  for (int i = 0; i < NUM_THREADS; i++) {
      pthread_join(threads[i], NULL);
  }
}


int main(int argc, char** argv)
{
  /* Retrieve problem size. */
  int n = N;

  /* Variable declaration/allocation. */
  POLYBENCH_2D_ARRAY_DECL(L, DATA_TYPE, N, N, n, n);
  POLYBENCH_1D_ARRAY_DECL(x, DATA_TYPE, N, n);
  POLYBENCH_1D_ARRAY_DECL(b, DATA_TYPE, N, n);


  /* Initialize array(s). */
  init_array (n, POLYBENCH_ARRAY(L), POLYBENCH_ARRAY(x), POLYBENCH_ARRAY(b));

  /* Start timer. */
  polybench_start_instruments;

  /* Run kernel. */
  kernel_trisolv_parallel (n, POLYBENCH_ARRAY(L), POLYBENCH_ARRAY(x), POLYBENCH_ARRAY(b));

  /* Stop and print timer. */
  polybench_stop_instruments;
  polybench_print_instruments;

  // Imprimir a matriz L
/*printf("Matriz L:\n");
for (int i = 0; i < n; i++) {
  printf("|");
    for (int j = 0; j < n; j++) {
        printf("%f ", *L[i][j]);
    }
    printf("|\n");
}

// Imprimir o vetor resultado x
printf("Vetor resultado x:\n");
for (int i = 0; i < n; i++) {
    printf("%f ", *x[i]);
}
printf("\n");*/


  /* Prevent dead-code elimination. All live-out data must be printed
     by the function call in argument. */
  polybench_prevent_dce(print_array(n, POLYBENCH_ARRAY(x)));

  /* Be clean. */
  POLYBENCH_FREE_ARRAY(L);
  POLYBENCH_FREE_ARRAY(x);
  POLYBENCH_FREE_ARRAY(b);

  return 0;
}
