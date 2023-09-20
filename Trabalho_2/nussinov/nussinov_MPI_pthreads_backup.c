#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <mpi.h>
#include <pthread.h>

/* Include polybench common header. */
#include "polybench.h"

/* Include benchmark-specific header. */
#include "nussinov.h"

/* RNA bases represented as chars, range is [0,3] */
typedef char base;

int NUM_THREADS_PER_PROCESS;

pthread_barrier_t barrier;

typedef struct {
    int thread_id;
    int rank;
    int n;
    base *seq;
    DATA_TYPE (*table)[N];
} hybrid_args;

#define match(b1, b2) (((b1)+(b2)) == 3 ? 1 : 0)
#define max_score(s1, s2) ((s1 >= s2) ? s1 : s2)

static
void init_array (int n,
                 base POLYBENCH_1D(seq,N,n),
		 DATA_TYPE POLYBENCH_2D(table,N,N,n,n))
{
    int i, j;

    //base is AGCT/0..3
    for (i = 0; i < n; i++) {
        seq[i] = (base)((i + 1) % 4);
    }

    for (i = 0; i < n; i++)
        for (j = 0; j < n; j++)
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

void *hybrid_nussinov(void *arg) {
    hybrid_args *args = (hybrid_args *)arg;
    int i, j, k;
    int num_processes;
    int num_thread = args->thread_id;
    int n = args->n;
    base *seq = args->seq;
    DATA_TYPE (*table)[N] = args->table;

    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);

    int global_id = args->rank * NUM_THREADS_PER_PROCESS + num_thread;

    for (i = n - 1; i >= 0; i--) {
        for (j = i + 1 + global_id; j < n; j += num_processes * NUM_THREADS_PER_PROCESS) {
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
        MPI_Barrier(MPI_COMM_WORLD);
        for (int src = 0; src < num_processes; src++) {
            for (j = i+1+src; j < n; j += num_processes) {
                MPI_Bcast(&table[i][j], 1, MPI_DOUBLE, src, MPI_COMM_WORLD);
            }
        }
    }
    return NULL;
}

int main(int argc, char** argv)
{
    /* Start timer. */
    polybench_start_instruments;
    /* Retrieve problem size. */
    int n = N;

    /* Variable declaration/allocation. */
    POLYBENCH_1D_ARRAY_DECL(seq, base, N, n);
    POLYBENCH_2D_ARRAY_DECL(table, DATA_TYPE, N, N, n, n);

    /* Initialize array(s). */
    init_array(n, POLYBENCH_ARRAY(seq), POLYBENCH_ARRAY(table));
    
    int rank, num_processes;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);

    NUM_THREADS_PER_PROCESS = 3; // Set appropriately, possibly from argv or another source


    pthread_t threads[NUM_THREADS_PER_PROCESS];
    hybrid_args args[NUM_THREADS_PER_PROCESS];
    pthread_barrier_init(&barrier, NULL, NUM_THREADS_PER_PROCESS);

    for (int t = 0; t < NUM_THREADS_PER_PROCESS; t++) {
        args[t].thread_id = t;
        args[t].rank = rank;
        args[t].n = n;
        args[t].seq = POLYBENCH_ARRAY(seq);
        args[t].table = POLYBENCH_ARRAY(table);
        pthread_create(&threads[t], NULL, hybrid_nussinov, &args[t]);
    }

    for (int t = 0; t < NUM_THREADS_PER_PROCESS; t++) {
        pthread_join(threads[t], NULL);
    }

    if (rank == 0) {
        printf("\nRESULTADO: " DATA_PRINTF_MODIFIER "\n", (*table)[0][N-1]);
    }

    /* Prevent dead-code elimination. All live-out data must be printed
    by the function call in argument. */
    polybench_prevent_dce(print_array(n, POLYBENCH_ARRAY(table)));

    /* Be clean. */
    POLYBENCH_FREE_ARRAY(seq);
    POLYBENCH_FREE_ARRAY(table);

    MPI_Finalize();
    if(rank == 0){
        polybench_stop_instruments;    
        polybench_print_instruments;
    }
    return 0;
}
