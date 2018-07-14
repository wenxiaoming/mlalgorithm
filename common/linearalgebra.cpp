#include "linearalgebra.h"
#include <assert.h>
#include <string.h>

//the following code is from opencv/modules/core/src/lapack.cpp
//it is used to calculate the eigenvalues and eigenvectors of matrix
template<typename _Tp> static inline _Tp* alignPtr(_Tp* ptr, int n=(int)sizeof(_Tp))
{
	assert((n & (n - 1)) == 0); // n is a power of 2
	return (_Tp*)(((size_t)ptr + n-1) & -n);
}

static inline size_t alignSize(size_t sz, int n)
{
	assert((n & (n - 1)) == 0); // n is a power of 2
	return (sz + n-1) & -n;
}

template<typename _Tp> static inline _Tp hypot(_Tp a, _Tp b)
{
	a = std::abs(a);
	b = std::abs(b);
	if( a > b )
	{
		b /= a;
		return a*std::sqrt(1 + b*b);
	}
	if( b > 0 )
	{
		a /= b;
		return b*std::sqrt(1 + a*a);
	}
	return 0;
}

	template<typename _Tp> bool
JacobiImpl_( _Tp* A, size_t astep, _Tp* W, _Tp* V, size_t vstep, int n, uchar* buf )
{
	const _Tp eps = std::numeric_limits<_Tp>::epsilon();
	int i, j, k, m;

	astep /= sizeof(A[0]);
	if( V )
	{
		vstep /= sizeof(V[0]);
		for( i = 0; i < n; i++ )
		{
			for( j = 0; j < n; j++ )
				V[i*vstep + j] = (_Tp)0;
			V[i*vstep + i] = (_Tp)1;
		}
	}

	int iters, maxIters = n*n*30;

	int* indR = (int*)alignPtr(buf, sizeof(int));
	int* indC = indR + n;
	_Tp mv = (_Tp)0;

	for( k = 0; k < n; k++ )
	{
		W[k] = A[(astep + 1)*k];
		if( k < n - 1 )
		{
			for( m = k+1, mv = std::abs(A[astep*k + m]), i = k+2; i < n; i++ )
			{
				_Tp val = std::abs(A[astep*k+i]);
				if( mv < val )
					mv = val, m = i;
			}
			indR[k] = m;
		}
		if( k > 0 )
		{
			for( m = 0, mv = std::abs(A[k]), i = 1; i < k; i++ )
			{
				_Tp val = std::abs(A[astep*i+k]);
				if( mv < val )
					mv = val, m = i;
			}
			indC[k] = m;
		}
	}

	if( n > 1 ) for( iters = 0; iters < maxIters; iters++ )
	{
		// find index (k,l) of pivot p
		for( k = 0, mv = std::abs(A[indR[0]]), i = 1; i < n-1; i++ )
		{
			_Tp val = std::abs(A[astep*i + indR[i]]);
			if( mv < val )
				mv = val, k = i;
		}
		int l = indR[k];
		for( i = 1; i < n; i++ )
		{
			_Tp val = std::abs(A[astep*indC[i] + i]);
			if( mv < val )
				mv = val, k = indC[i], l = i;
		}

		_Tp p = A[astep*k + l];
		if( std::abs(p) <= eps )
			break;
		_Tp y = (_Tp)((W[l] - W[k])*0.5);
		_Tp t = std::abs(y) + hypot(p, y);
		_Tp s = hypot(p, t);
		_Tp c = t/s;
		s = p/s; t = (p/t)*p;
		if( y < 0 )
			s = -s, t = -t;
		A[astep*k + l] = 0;

		W[k] -= t;
		W[l] += t;

		_Tp a0, b0;

#undef rotate
#define rotate(v0, v1) a0 = v0, b0 = v1, v0 = a0*c - b0*s, v1 = a0*s + b0*c

		// rotate rows and columns k and l
		for( i = 0; i < k; i++ )
			rotate(A[astep*i+k], A[astep*i+l]);
		for( i = k+1; i < l; i++ )
			rotate(A[astep*k+i], A[astep*i+l]);
		for( i = l+1; i < n; i++ )
			rotate(A[astep*k+i], A[astep*l+i]);

		// rotate eigenvectors
		if( V )
			for( i = 0; i < n; i++ )
				rotate(V[vstep*k+i], V[vstep*l+i]);

#undef rotate

		for( j = 0; j < 2; j++ )
		{
			int idx = j == 0 ? k : l;
			if( idx < n - 1 )
			{
				for( m = idx+1, mv = std::abs(A[astep*idx + m]), i = idx+2; i < n; i++ )
				{
					_Tp val = std::abs(A[astep*idx+i]);
					if( mv < val )
						mv = val, m = i;
				}
				indR[idx] = m;
			}
			if( idx > 0 )
			{
				for( m = 0, mv = std::abs(A[idx]), i = 1; i < idx; i++ )
				{
					_Tp val = std::abs(A[astep*i+idx]);
					if( mv < val )
						mv = val, m = i;
				}
				indC[idx] = m;
			}
		}
	}

	// sort eigenvalues & eigenvectors
	for( k = 0; k < n-1; k++ )
	{
		m = k;
		for( i = k+1; i < n; i++ )
		{
			if( W[m] < W[i] )
				m = i;
		}
		if( k != m )
		{
			std::swap(W[m], W[k]);
			if( V )
				for( i = 0; i < n; i++ )
					std::swap(V[vstep*m + i], V[vstep*k + i]);
		}
	}

	return true;
}

static bool Jacobi( float* S, size_t sstep, float* e, float* E, size_t estep, int n, uchar* buf )
{
	return JacobiImpl_(S, sstep, e, E, estep, n, buf);
}

bool eigen( const std::vector<std::vector<float>>&  _src, std::vector<std::vector<float>>&  _evals,
		std::vector<std::vector<float>>&  _evects )
{
	int n = _src.size();
	size_t elemSize = _src[0].size();
	size_t astep = alignSize(n*elemSize, 16);
	uchar* buf = (uchar*)malloc(n*astep + n*5*elemSize + 32);
	if(!buf)
		return false;

	uchar* v_buf = (uchar*)malloc(n*n*sizeof(float));
	if(!v_buf)
		return false;

	uchar* ptr = alignPtr(buf, 16);
	uchar* a_buf = ptr;
	uchar* w_buf = ptr + astep*n;
	ptr += astep*n + elemSize*n;

	for(int i = 0; i < _src.size(); i++)
		memcpy(a_buf + i*_src[0].size()*sizeof(float), (char*)&_src[i][0], _src[0].size()*sizeof(float));

	//print a_buf
	float* temp = (float*)a_buf;
	for(int i = 0; i < 4*4; i++)
		printf("%f ", temp[i]);

	bool ok = Jacobi((float*)a_buf, astep, (float*)w_buf, (float*)v_buf, n, n, ptr);

	//copy backto _evects
	float* v_buf_float = (float*)v_buf;
	for(int j = 0; j < n; j++)
	{
		std::vector<float> evect;
		for(int i = 0; i < n; i++)
		{
			printf("v_buf_float[%d]: %f\n", i, v_buf_float[i]);
			evect.push_back(v_buf_float[i]);
		}
		v_buf_float += n;
		_evects.push_back(evect);
	}
	//copy backto _evals
	float* w_buf_float = (float*)w_buf;
	std::vector<float> eval;
	for(int i = 0; i < n; i++)
	{
		printf("w_buf_float[%d]: %f\n", i, w_buf_float[i]);
		eval.push_back(w_buf_float[i]);
	}
	_evals.push_back(eval);
	free(buf);
	free(v_buf);
	return ok;
}

