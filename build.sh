#!/bin/bash

set -x
CC="g++ -g3 -O0 -Wall -std=c++11 "
CC="$CC -I naivebayes"

mkdir -p objs
rm -rf objs/*.o objs/*.a objs/*test*

$CC -c naivebayes/naivebayes.cpp -o objs/naivebayes.o

ar rcs objs/libml.a objs/*.o

g++ -g -Wall mlalgorithm_test.cpp -o objs/mlalgorithm_test objs/libml.a

