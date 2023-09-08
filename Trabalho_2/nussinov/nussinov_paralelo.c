/**
 * This version is stamped on May 10, 2016
 *
 * Contact:
 *   Louis-Noel Pouchet <pouchet.ohio-state.edu>
 *   Tomofumi Yuki <tomofumi.yuki.fr>
 *
 * Web address: http://polybench.sourceforge.net
 */
/* nussinov.c: this file is part of PolyBench/C */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include<pthread.h>

/* Include polybench common header. */
#include "polybench.h"

/* Include benchmark-specific header. */
#include "nussinov.h"

/* RNA bases represented as chars, range is [0,3] */
typedef char base;

int NUM_THREADS;

pthread_barrier_t barrier;

typedef struct {
    int num_thread;
    int n; //talvez isso n~ao precise, ja existe o _PB_N com o tamanho de n
    base *seq;
    DATA_TYPE (*table)[N];
} thread_args;

#define match(b1, b2) (((b1)+(b2)) == 3 ? 1 : 0)
#define max_score(s1, s2) ((s1 >= s2) ? s1 : s2)

/* Array initialization. */
static
void init_array (int n,
                 base POLYBENCH_1D(seq,N,n),
		 DATA_TYPE POLYBENCH_2D(table,N,N,n,n))
{
  int i, j;

  //base is AGCT/0..3
  for (i=0; i <n; i++) {
     seq[i] = (base)((i+1)%4);
  }

  for (i=0; i <n; i++)
     for (j=0; j <n; j++)
       table[i][j] = 0;
}


/* DCE code. Must scan the entire live-out data.
   Can be used also to check the correctness of the output. */
static
void print_array(int n,
		 DATA_TYPE POLYBENCH_2D(table,N,N,n,n))

{
  int i, j;
  int t = 0;

  POLYBENCH_DUMP_START;
  POLYBENCH_DUMP_BEGIN("table");
  for (i = 0; i < n; i++) {
    for (j = i; j < n; j++) {
      if (t % 20 == 0) fprintf (POLYBENCH_DUMP_TARGET, "\n");
      fprintf (POLYBENCH_DUMP_TARGET, DATA_PRINTF_MODIFIER, table[i][j]);
      t++;
    }
  }
  POLYBENCH_DUMP_END("table");
  POLYBENCH_DUMP_FINISH;
}


/* Main computational kernel. The whole function will be timed,
   including the call and return. */
/*
  Original version by Dave Wonnacott at Haverford College <davew@cs.haverford.edu>,
  with help from Allison Lake, Ting Zhou, and Tian Jin,
  based on algorithm by Nussinov, described in Allison Lake's senior thesis.
*/
void *compute_diagonal(void *arg) {
    thread_args *args = (thread_args *)arg;
    int i, j, k;
    int num_thread = args->num_thread;
    int n = args->n;
    base *seq = args->seq;
    DATA_TYPE (*table)[N] = args->table;

    #pragma scop

    for (i = n-1; i >= 0; i--) {
        for (j = i+1+num_thread; j < n; j += NUM_THREADS) { 
            if (j-1 >= 0)
                table[i][j] = max_score(table[i][j], table[i][j-1]);
            if (i+1 < n)
                table[i][j] = max_score(table[i][j], table[i+1][j]);

            if (j-1 >= 0 && i+1 < n) {
                if (i < j-1)
                    table[i][j] = max_score(table[i][j], table[i+1][j-1]+match(seq[i], seq[j]));
                else
                    table[i][j] = max_score(table[i][j], table[i+1][j-1]);
            }

            for (k = i+1; k < j; k++) {
                table[i][j] = max_score(table[i][j], table[i][k] + table[k+1][j]);
            }
        }
        pthread_barrier_wait(&barrier);
    }

    #pragma endscop

    //o resultado esta correto, mas os numeros principalmente perto da direita estao errados, talvez seja so a maneira de resolver

    return NULL;
}

static
void kernel_nussinov(int n, base POLYBENCH_1D(seq,N,n),
                     DATA_TYPE POLYBENCH_2D(table,N,N,n,n))
{
    pthread_t threads[NUM_THREADS];
    thread_args args[NUM_THREADS];
    int i;


    pthread_barrier_init(&barrier, NULL, NUM_THREADS);
    for (i = 0; i < NUM_THREADS; i++) {
        args[i].num_thread = i;
        args[i].n = n;
        args[i].seq = seq;
        args[i].table = table;
        pthread_create(&threads[i], NULL, &compute_diagonal, &args[i]);
    }

    for (i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    /*for(int i = 0; i< _PB_N; i++) { //printa a tabela
      printf("|");
      for(int j = 0; j< _PB_N; j++) {
         printf("%.2lf ", table[i][j]);
      }
      printf("|\n");
   }*/

   printf("\nRESULTADO: %.2lf\n", table[0][N-1]);
}


int main(int argc, char** argv)
{
  if (argc < 2) {
      fprintf(stderr, "Utilize o comando %s <numero_de_threads>\n", argv[0]);
      return 1;
  }

  NUM_THREADS = atoi(argv[1]);

  if (NUM_THREADS <= 0) {
    fprintf(stderr, "O numero de threads deve ser maior que 0!\n");
    return 1;
  }


  /* Retrieve problem size. */
  int n = N;

  /* Variable declaration/allocation. */
  POLYBENCH_1D_ARRAY_DECL(seq, base, N, n);
  POLYBENCH_2D_ARRAY_DECL(table, DATA_TYPE, N, N, n, n);

  /* Initialize array(s). */
  init_array (n, POLYBENCH_ARRAY(seq), POLYBENCH_ARRAY(table));

  /* Start timer. */
  polybench_start_instruments;

  /* Run kernel. */
  kernel_nussinov (n, POLYBENCH_ARRAY(seq), POLYBENCH_ARRAY(table));

  /* Stop and print timer. */
  polybench_stop_instruments;
  polybench_print_instruments;

  /* Prevent dead-code elimination. All live-out data must be printed
     by the function call in argument. */
  polybench_prevent_dce(print_array(n, POLYBENCH_ARRAY(table)));

  /* Be clean. */
  POLYBENCH_FREE_ARRAY(seq);
  POLYBENCH_FREE_ARRAY(table);

  return 0;
}
