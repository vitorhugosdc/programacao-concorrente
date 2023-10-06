#!/bin/bash

programa="./nussinov_mpi_pthreads"
quantidade=$4

for (( i=1; i<=quantidade; i++ )); do
    echo "Executando repetição $i para $1 e com $2 processos:"
    comando="perf stat -e cpu-clock,task-clock,context-switches,cpu-migrations,page-faults,branches,branch-misses,cache-references,cache-misses,cycles,instructions mpirun -np $2 $programa -d $1 -t $3"
    $comando
    echo "----------------------------------------"
done
