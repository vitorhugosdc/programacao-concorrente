CC = gcc
CFLAGS = -O3 -DPOLYBENCH_TIME -I./utilities
MPI = mpicc
MPIFLAGS = -g -Wall
PATH_SRC = ./
UTILITIES_PATH = ./utilities/
OBJS = nussinov_seq nussinov_pthreads nussinov_mpi

all: $(OBJS)

nussinov_seq: nussinov_seq.c polybench.c
	@$(CC) $(CFLAGS) -o nussinov_seq $(PATH_SRC)nussinov_seq.c $(UTILITIES_PATH)polybench.c

nussinov_pthreads: nussinov_pthreads.c polybench.c
	@$(CC) $(CFLAGS) -o nussinov_pthreads $(PATH_SRC)nussinov_pthreads.c $(UTILITIES_PATH)polybench.c

nussinov_mpi: nussinov_mpi.c polybench.c
	@$(MPI) $(CFLAGS) $(MPIFLAGS) -o nussinov_mpi $(PATH_SRC)nussinov_mpi.c $(UTILITIES_PATH)polybench.c

clean:
	rm -f nussinov_seq nussinov_pthreads nussinov_mpi
