#!/bin/bash

programa="./nussinov_pthreads"
quantidade=$3

for (( i=1; i<=quantidade; i++ )); do
    echo "Executando repetição $i para $1:"
    comando="perf stat -e cpu-clock,task-clock,context-switches,cpu-migrations,page-faults,branches,branch-misses,cache-references,cache-misses,cycles,instructions $programa -d $1 -t $2"
    $comando
    echo "----------------------------------------"
done
