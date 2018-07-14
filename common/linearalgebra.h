#ifndef LINEAR_ALGEBRA
#define LINEAR_ALGEBRA
#include <vector>
#include <algorithm>
#include <stdint.h>

typedef unsigned char uchar;
bool eigen( const std::vector<std::vector<float>>&  _src, std::vector<std::vector<float>>&  _evals,
		    std::vector<std::vector<float>>&  _evects );

#endif
