#!/bin/bash
CMD="./cmake-build-release/partial_sum"

for OMP_NUM_THREADS in {1..8}; do
  printf ";%d" "$OMP_NUM_THREADS"
done

for (( M=25000; M<=50000; M+=5000 )); do
  printf "\n%d;" "$M"
  for OMP_NUM_THREADS in {1..8}; do
    RESULT=$("$CMD" -n 1 -m "$M")
    RESULT=${RESULT//Elapsed time: /}
    RESULT=${RESULT// seconds/}

    printf "%s;" "$RESULT"
  done
done
