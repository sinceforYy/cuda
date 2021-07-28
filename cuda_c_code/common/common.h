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

#endif
