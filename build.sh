#!/bin/bash

set -x
CC="gcc -g3 -O0 -Wall -m32 "
CC="$CC -I naivebayes"

mkdir -p objs
rm -rf objs/*.o objs/*.a objs/*test*

$CC -c naivebayes/naivebayes.cpp -o objs/naivebayes.o

ar rcs objs/libml.a objs/*.o

gcc -m32 -g -Wall mlalgorithm_test.cpp -o objs/mlalgorithm_test objs/libml.a

