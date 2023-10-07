#include <stdio.h>
#include <string.h>
#include <math.h>
#include <argp.h>
#include <stdlib.h>
#include <pthread.h>

#include "polybench.h"

struct arguments
{
    int size;
    int debug;
    int num_threads;
};

static struct argp_option options[] = {
    {"size", 'd', "SIZE", 0, "Specify matrix size (small, medium, or large)"},
    {"debug", 'D', 0, 0, "Print entire resultant matrix"},
    {"threads", 't', "NUM", 0, "Number of threads"},
    {"help", 'h', 0, 0, "Show this help message"},
    {0}
};

static error_t parse_opt(int key, char *arg, struct argp_state *state)
{
    struct arguments *arguments = state->input;

    switch (key)
    {
    case 'd':
        if (strcmp(arg, "small") == 0)
            arguments->size = 7260;
        else if (strcmp(arg, "medium") == 0)
            arguments->size = 8800;
        else if (strcmp(arg, "large") == 0)
            arguments->size = 9950;
        else
        {
            fprintf(stderr, "Size is not a valid option: %s\n", arg);
            return ARGP_ERR_UNKNOWN;
        }
        break;
    case 'D':
        arguments->debug = 1;
        break;
    case 't':
        arguments->num_threads = atoi(arg);
        if (arguments->num_threads <= 0)
        {
            fprintf(stderr, "Number of threads must be greater than 0!\n");
            return ARGP_ERR_UNKNOWN;
        }
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

static struct argp argp = {options, parse_opt, NULL, "NUSSINOV Help"};

pthread_barrier_t barrier;

typedef char base;

base *seq;
double **table;

int NUM_THREADS;

#define match(b1, b2) (((b1)+(b2)) == 3 ? 1 : 0)
#define max_score(s1, s2) ((s1 >= s2) ? s1 : s2)

typedef struct {
    int num_thread;
    int n;
} thread_args;

void allocateMatrix(int n)
{
    seq = (base *)malloc(n * sizeof(base));
    table = (double **)malloc(n * sizeof(double *));
    for (int i = 0; i < n; i++)
    {
        table[i] = (double *)malloc(n * sizeof(double));
    }
}

void freeMatrix(int n)
{
    for (int i = 0; i < n; i++)
    {
        free(table[i]);
    }
    free(table);
    free(seq);
}

static void init_array(int n)
{
    int i, j;

    for (i = 0; i < n; i++)
    {
        seq[i] = (base)((i + 1) % 4);
    }

    for (i = 0; i < n; i++)
        for (j = 0; j < n; j++)
            table[i][j] = 0;
}

static void print_array(int n)
{
    int i, j;

    for (i = 0; i < n; i++)
    {
        for (j = i; j < n; j++)
        {
            fprintf(stdout, "%.2lf ", table[i][j]);
        }
        fprintf(stdout, "\n");
    }
}

void *kernel_nussinov(void *arg)
{
    thread_args *args = (thread_args *)arg;
    int num_thread = args->num_thread;
    int n = args->n;
    int i, j, k;

    for (i = n - 1; i >= 0; i--)
    {
        for (j = i + 1 + num_thread; j < n; j += NUM_THREADS)
        {
            if (j - 1 >= 0)
                table[i][j] = max_score(table[i][j], table[i][j - 1]);
            if (i + 1 < n)
                table[i][j] = max_score(table[i][j], table[i + 1][j]);

            if (j - 1 >= 0 && i + 1 < n)
            {
                if (i < j - 1)
                    table[i][j] = max_score(table[i][j], table[i + 1][j - 1] + match(seq[i], seq[j]));
                else
                    table[i][j] = max_score(table[i][j], table[i + 1][j - 1]);
            }

            for (k = i + 1; k < j; k++)
            {
                table[i][j] = max_score(table[i][j], table[i][k] + table[k + 1][j]);
            }
        }
        pthread_barrier_wait(&barrier);
    }

    return NULL;
}

int main(int argc, char **argv)
{
    polybench_start_instruments;
    struct arguments arguments;
    arguments.size = 0;
    arguments.debug = 0;
    arguments.num_threads = 0;

    argp_parse(&argp, argc, argv, 0, 0, &arguments);

    if (!arguments.size)
    {
        fprintf(stderr, "The -d argument is required. Use -h to see the commands.\n");
        exit(1);
    }

    if (!arguments.num_threads)
    {
        fprintf(stderr, "O argumento -t é obrigatório e deve ser maior que 0. Use -h para ver os comandos.\n");
        exit(1);
    }

    int n = arguments.size;
    NUM_THREADS = arguments.num_threads;

    allocateMatrix(n);
    init_array(n);

    pthread_t threads[arguments.num_threads];
    thread_args args[arguments.num_threads];
    int i;

    pthread_barrier_init(&barrier, NULL, arguments.num_threads);

    for (i = 0; i < arguments.num_threads; i++) {
        args[i].num_thread = i;
        args[i].n = n;
        pthread_create(&threads[i], NULL, kernel_nussinov, &args[i]);
    }

    for (i = 0; i < arguments.num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("\nRESULT: %.2lf\n", table[0][n - 1]);

    if (arguments.debug)
        print_array(n);

    freeMatrix(n);

    polybench_stop_instruments;
    polybench_print_instruments;
    return 0;
}
