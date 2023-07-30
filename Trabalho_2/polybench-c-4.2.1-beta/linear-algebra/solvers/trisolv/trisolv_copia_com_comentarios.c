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

/* Include polybench common header. */
#include <polybench.h>

/* Include benchmark-specific header. */
#include "trisolv.h"


/* Array initialization. */
static
void init_array(int n,
		DATA_TYPE POLYBENCH_2D(L,N,N,n,n), //POLYBENCH_2D é um vetor de 2 dimensões, ou seja, recebe uma matriz chamada L, com linhas de tamanho N, colunas de tamanho N, n e n são iguais ao tamanho da matriz
		DATA_TYPE POLYBENCH_1D(x,N,n), //POLYBENCH_1D é um vetor normal, ou seja, vetor de tamanho N onde n é o tamanho do vetor pra usar no for
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
void kernel_trisolv(int n, 
		    DATA_TYPE POLYBENCH_2D(L,N,N,n,n), //POLYBENCH_2D é um vetor de 2 dimensões, ou seja, recebe uma matriz chamada L, com linhas de tamanho N, colunas de tamanho N, n e n são iguais ao tamanho da matriz
		    DATA_TYPE POLYBENCH_1D(x,N,n), //POLYBENCH_1D é um vetor normal, ou seja, vetor de tamanho N onde n é o tamanho do vetor pra usar no for
		    DATA_TYPE POLYBENCH_1D(b,N,n))
{
  int i, j;

#pragma scop
  for (i = 0; i < _PB_N; i++) //forward substitution o nome desse cálculo de matriz triângular para lower triangular matrix
    {                         //_PB_N é o tamanho N da matriz
      x[i] = b[i]; //x é o vetor solução para o sistema linear formado pela matriz, b é o vetor
      for (j = 0; j <i; j++)
        x[i] -= L[i][j] * x[j];
      x[i] = x[i] / L[i][i];
    }
#pragma endscop

}


int main(int argc, char** argv)
{
  /* Retrieve problem size. */
  int n = N;

  /* Variable declaration/allocation. */

  //DATA_TYPE (trisolv.h) são os tipos de dado do vetor, seja int, float ou double, se não for informado o default é double

  POLYBENCH_2D_ARRAY_DECL(L, DATA_TYPE, N, N, n, n); //declara um vetor de 2 dimensões (matriz), de nome L, do tipo inserido, N linhas, N colunas, tamanho n e n para interar sobre linhas e colunas
  POLYBENCH_1D_ARRAY_DECL(x, DATA_TYPE, N, n);  //declara 1 vetor de 1 dimensão
  POLYBENCH_1D_ARRAY_DECL(b, DATA_TYPE, N, n);  //declara 1 vetor de 1 dimensão


  /* Initialize array(s). */
  init_array (n, POLYBENCH_ARRAY(L), POLYBENCH_ARRAY(x), POLYBENCH_ARRAY(b));

  /* Start timer. */
  polybench_start_instruments;

  /* Run kernel. */
  kernel_trisolv (n, POLYBENCH_ARRAY(L), POLYBENCH_ARRAY(x), POLYBENCH_ARRAY(b));

  /* Stop and print timer. */
  polybench_stop_instruments;
  polybench_print_instruments;

  /* Prevent dead-code elimination. All live-out data must be printed
     by the function call in argument. */
  polybench_prevent_dce(print_array(n, POLYBENCH_ARRAY(x)));

  /* Be clean. */
  POLYBENCH_FREE_ARRAY(L);
  POLYBENCH_FREE_ARRAY(x);
  POLYBENCH_FREE_ARRAY(b);

  return 0;
}
