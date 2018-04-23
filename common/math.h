#ifndef MATH_H
#define MATH_H

#include <stdio.h>
#include <string.h>
#include <vector>
#include <string>
#include <algorithm>
#include <map>

using namespace std;

bool samesign(float a, float b);
vector<float> add(vector<float> a, vector<float> b);
float multiply(vector<float> a, vector<float> b);
vector<float> dotproduct(vector<float> a, vector<float> b);
vector<float> multiply(vector<float> a, float b);
float sum(vector<float> a);
void normalize(vector<float>& a);
vector<float> comparesign(vector<float> a, vector<float> b);

#endif