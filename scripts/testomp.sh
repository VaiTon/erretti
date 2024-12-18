#!/bin/sh
cmake --build build

for i in 1 2 4 8 16; do
    export OMP_NUM_THREADS=$i
    echo "Test with OMP_NUM_THREADS=$OMP_NUM_THREADS"
    time ./build/rtracer
done
