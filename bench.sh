#!/bin/sh

N=$1
M=$2
G=$3
T=$4

./build/filter-kruskal -n $N -m $M -t $T -g $G -s filterkruskalnaive
./build/filter-kruskal -n $N -m $M -t $T -g $G -s filterkruskalskewed
./build/filter-kruskal -n $N -m $M -t $T -g $G -s prim
./build/filter-kruskal -n $N -m $M -t $T -g $G -s samplekruskal
./build/filter-kruskal -n $N -m $M -t $T -g $G -s kruskal