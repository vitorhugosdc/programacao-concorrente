CC = gcc
CFLAGS = -O3 -DPOLYBENCH_TIME
MPI = mpicc
MPIFLAGS = -g -Wall
PATH_SRC = ./
OBJS = nussinov_seq nussinov_pthreads nussinov_mpi nussinov_mpi_pthreads

all: $(OBJS)

nussinov_seq: nussinov_seq.c polybench.c
	@$(CC) $(CFLAGS) -o nussinov_seq $(PATH_SRC)nussinov_seq.c $(PATH_SRC)polybench.c

nussinov_pthreads: nussinov_pthreads.c polybench.c
	@$(CC) $(CFLAGS) -o nussinov_pthreads $(PATH_SRC)nussinov_pthreads.c $(PATH_SRC)polybench.c

nussinov_mpi: nussinov_mpi.c polybench.c
	@$(MPI) $(CFLAGS) $(MPIFLAGS) -o nussinov_mpi $(PATH_SRC)nussinov_mpi.c $(PATH_SRC)polybench.c

nussinov_mpi_pthreads: nussinov_mpi_pthreads.c polybench.c
	@$(MPI) $(CFLAGS) $(MPIFLAGS) -o nussinov_mpi_pthreads $(PATH_SRC)nussinov_mpi_pthreads.c $(PATH_SRC)polybench.c

clean:
	rm -f nussinov_seq nussinov_pthreads nussinov_mpi nussinov_mpi_pthreads
