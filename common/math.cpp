#include "math.h"

vector<float> add(vector<float> a, vector<float> b)
{
    int i = 0;
    vector<float> result;
    for (; i < a.size(); i++) {
        float value = a[i] + b[i];
        result.push_back(value);
    }
    return result;
}

float multiply(vector<float> a, vector<float> b)
{
    int i = 0;
    float result = 0.0f;
    for (; i < a.size(); i++) {
        float value = a[i] * b[i];
        result += value;
    }
    return result;
}

vector<float> multiply(vector<float> a, float b)
{
    int i = 0;
    vector<float> result;
    for (; i < a.size(); i++) {
        float value = a[i] * b;
        result.push_back(value);
    }
    return result;
}

float sum(vector<float> a)
{
    int i = 0;
    float result = 0.0f;
    for (; i < a.size(); i++) {
        result += a[i];
    }
    return result;
}

void normalize(vector<float>& a)
{
    int i = 0;
    float total = sum(a);
    for (; i < a.size(); i++) {
        a[i] = a[i] / total;
    }
}

bool samesign(float a, float b)
{
    if (a < 0 && b < 0)
        return true;
    else if (a > 0 && b > 0)
        return true;
    else
        return false;
}

vector<float> comparesign(vector<float> a, vector<float> b)
{
    int i = 0;
    vector<float> result;
    for (; i < a.size(); i++) {
        float value = samesign(a[i], b[i]) ? 0: 1;
        result.push_back(value);
    }
    return result;
}