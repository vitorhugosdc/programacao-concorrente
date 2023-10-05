CC = gcc
CFLAGS = -O3 -DPOLYBENCH_TIME
MPI = mpicc
MPIFLAGS = -g -Wall
PATH_SRC = ./
OBJS = nussinov_seq nussinov_pthreads nussinov_MPI nussinov_MPI_pthreads

all: $(OBJS)

nussinov_seq: nussinov_seq.c polybench.c
	@$(CC) $(CFLAGS) -o nussinov_seq $(PATH_SRC)nussinov_seq.c $(PATH_SRC)polybench.c

nussinov_pthreads: nussinov_pthreads.c polybench.c
	@$(CC) $(CFLAGS) -o nussinov_pthreads $(PATH_SRC)nussinov_pthreads.c $(PATH_SRC)polybench.c

nussinov_MPI: nussinov_MPI.c polybench.c
	@$(MPI) $(CFLAGS) $(MPIFLAGS) -o nussinov_MPI $(PATH_SRC)nussinov_MPI.c $(PATH_SRC)polybench.c

nussinov_MPI_pthreads: nussinov_MPI_pthreads.c polybench.c
	@$(MPI) $(CFLAGS) $(MPIFLAGS) -o nussinov_MPI_pthreads $(PATH_SRC)nussinov_MPI_pthreads.c $(PATH_SRC)polybench.c

clean:
	rm -f nussinov_seq nussinov_pthreads nussinov_MPI nussinov_MPI_pthreads
