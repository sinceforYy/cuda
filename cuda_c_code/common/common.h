#include <sys/time.h>

#ifndef _COMMON_H
#define _COMMON_H

#define CHECK(call)										\
{												\
	const cudaError_t error = call;								\
	if (error != cudaSuccess)								\
	{											\
		fprintf(stderr, "Error:%s:%d, code:%d, reason:%s\n", __FILE__, __LINE__,	\
				error, cudaGetErrorString(error));				\
		exit(1);									\
	}											\
}

inline double seconds()
{
	struct timeval tp;
	struct timezone tzp;
	int i = gettimeofday(&tp, &tzp);
	return ((double)tp.tv_sec + (double)tp.tv_usec * 1.e-6);
}

#endif
