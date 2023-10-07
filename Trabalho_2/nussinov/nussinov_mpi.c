#include <stdio.h>
#include <string.h>
#include <math.h>
#include <argp.h>
#include <stdlib.h>
#include <mpi.h>

#include "polybench.h"

/* Definição dos argumentos e suas opções */
struct arguments {
    int size;    // tamanho da matriz
    int debug;   // debug
};

static struct argp_option options[] = {
    {"size", 'd', "SIZE", 0, "Specify matrix size (small, medium, or large)"},
    {"debug", 'D', 0, 0, "Print debug information"},
    {"help", 'h', 0, 0, "Show help message"},
    {0}
};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    struct arguments *arguments = state->input;

    switch (key) {
        case 'd':
            if (strcmp(arg, "small") == 0)
                arguments->size = 7260;
            else if (strcmp(arg, "medium") == 0)
                arguments->size = 8800;
            else if (strcmp(arg, "large") == 0)
                arguments->size = 9950;
            else
            {
                fprintf(stderr, "Tamanho especificado não é válido: %s\n", arg);
                return ARGP_ERR_UNKNOWN;
            }
            break;
        case 'D':
            arguments->debug = 1;
            break;
        case 'h':
            argp_state_help(state, stdout, ARGP_HELP_STD_HELP);
            break;
        case ARGP_KEY_ARG:
            return 0;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static struct argp argp = {options, parse_opt, NULL, "NUSSINOV Description"};

/* RNA bases represented as chars, range is [0,3] */
typedef char base;

base *seq;
double **table;

#define match(b1, b2) (((b1)+(b2)) == 3 ? 1 : 0)
#define max_score(s1, s2) ((s1 >= s2) ? s1 : s2)

void allocateMatrix(int n) {
    seq = (base *)malloc(n * sizeof(base));
    table = (double **)malloc(n * sizeof(double *));
    for (int i = 0; i < n; i++) {
        table[i] = (double *)malloc(n * sizeof(double));
    }
}

void freeMatrix(int n) {
    for (int i = 0; i < n; i++) {
        free(table[i]);
    }
    free(table);
    free(seq);
}

/* Array initialization. */
static void init_array(int n) {
    int i, j;

    for (i = 0; i < n; i++) {
        seq[i] = (base)((i + 1) % 4);
    }

    for (i = 0; i < n; i++)
        for (j = 0; j < n; j++)
            table[i][j] = 0;
}

static void print_array(int n) {
    int i, j;

    for (i = 0; i < n; i++) {
        for (j = i; j < n; j++) {
            fprintf(stdout, "%.2lf ", table[i][j]);
        }
        fprintf(stdout, "\n");
    }
}

void kernel_nussinov(int n) {
    int i, j, k;
    int rank, num_processes;
    
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);

    for (i = n-1; i >= 0; i--) {
        for (j = i+1+rank; j < n; j += num_processes) {
            if (j-1 >= 0)
                table[i][j] = max_score(table[i][j], table[i][j-1]);
            if (i+1 < n)
                table[i][j] = max_score(table[i][j], table[i+1][j]);

            if (j-1 >= 0 && i+1 < n) {
                if (i < j-1)
                    table[i][j] = max_score(table[i][j], table[i+1][j-1] + match(seq[i], seq[j]));
                else
                    table[i][j] = max_score(table[i][j], table[i+1][j-1]);
            }

            for (k = i+1; k < j; k++) {
                table[i][j] = max_score(table[i][j], table[i][k] + table[k+1][j]);
            }
        }
        for (int src = 0; src < num_processes; src++) {
            for (j = i+1+src; j < n; j += num_processes) {
                MPI_Bcast(&table[i][j], 1, MPI_DOUBLE, src, MPI_COMM_WORLD);
            }
        }
    }
}

int main(int argc, char** argv) {
    polybench_start_instruments;

    struct arguments arguments;
    arguments.size = 0;
    arguments.debug = 0;

    argp_parse(&argp, argc, argv, 0, 0, &arguments);

    if (!arguments.size) {
        fprintf(stderr, "O argumento -d é obrigatório. Use -h para ver os comandos.\n");
        exit(1);
    }

    int n = arguments.size;

    allocateMatrix(n);
    init_array(n);

    int rank;
    MPI_Init(&argc, &argv);

    kernel_nussinov(n);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    
    MPI_Finalize();
    if (rank == 0) {
      printf("\nRESULTADO: %.2lf\n", table[0][n-1]);
      if (arguments.debug) {
        print_array(n);
      }
    }    
    freeMatrix(n);
    polybench_stop_instruments;
    polybench_print_instruments;
    return 0;
}