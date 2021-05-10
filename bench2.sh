#! /bin/sh

G=$1

./bench.sh 60000 10000 $G 100
./bench.sh 60000 20000 $G 100
./bench.sh 60000 40000 $G 100
./bench.sh 60000 80000 $G 100
./bench.sh 60000 160000 $G 100
./bench.sh 60000 320000 $G 100
./bench.sh 60000 640000 $G 10
./bench.sh 60000 1280000 $G 10
./bench.sh 60000 2560000 $G 10
./bench.sh 60000 5120000 $G 10
./bench.sh 60000 10240000 $G 5
./bench.sh 60000 20480000 $G 5
./bench4.sh 60000 40960000 $G 1
./bench4.sh 60000 81920000 $G 1
./bench4.sh 60000 163840000 $G 1
./bench4.sh 60000 327680000 $G 1
./bench4.sh 60000 655360000 $G 1
./bench4.sh 60000 1310720000 $G 1
./bench4.sh 60000 2621440000 $G 1

