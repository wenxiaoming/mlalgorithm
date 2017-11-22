#!/bin/bash

set -x
CC="g++ -g3 -O0 -Wall -std=c++11 "
CC="$CC -I naivebayes"

mkdir -p objs
rm -rf objs/*.o objs/*.a objs/*test*

$CC -c naivebayes/naivebayes.cpp -o objs/naivebayes.o
$CC -c apriori/apriori.cpp -o objs/apriori.o
$CC -c fpgrowth/fpgrowth.cpp -o objs/fpgrowth.o
$CC -c knn/knn.cpp -o objs/knn.o
$CC -c svm/svm.cpp -o objs/svm.o
$CC -c decisiontree/decisiontree.cpp -o objs/decisiontree.o
$CC -c cart/cart.cpp -o objs/cart.o

ar rcs objs/libml.a objs/*.o

g++ -g -Wall -std=c++11 mlalgorithm_test.cpp -o objs/mlalgorithm_test objs/libml.a

