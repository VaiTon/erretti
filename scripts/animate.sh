#!/bin/sh

mkdir -p frames
rm -f frames/*

program=build/rtracer

# collect all y
y_max=10

# divide by 10
y_values=$(seq 0 0.2 $y_max)
y_values=$(echo $y_values | tr ' ' '\n' | tr ',' '.')
y_idx=$(seq 0 0.2 $y_max | wc -l)
y_idx=$(seq 0 1 $(($y_idx - 1)))

set OMP_NUM_THREADS=2
j=$(nproc --all)
parallel --progress -j$j  $program frames/frame{2}.ppm 1280 720 10 0 {1} 0 ::: $y_values :::+ $y_idx

# replace frame0.1.ppm with frame01.ppm
find frames -name "*.ppm" | while read f; do
    mv "$f" "$(echo $f | sed 's/\([0-9]\.[0-9]\)/0\1/g')"
done

set -x
ffmpeg -r 10 -i frames/frame%d.ppm -cpu-used 8 -y output.webm
ffmpeg -i output.webm -filter:v minterpolate -r 30 -y output_blended.webm
