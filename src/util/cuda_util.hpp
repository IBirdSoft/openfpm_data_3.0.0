/*
 * cuda_util.hpp
 *
 *  Created on: Jun 13, 2018
 *      Author: i-bird
 */

#ifndef OPENFPM_DATA_SRC_UTIL_CUDA_UTIL_HPP_
#define OPENFPM_DATA_SRC_UTIL_CUDA_UTIL_HPP_

#include "config.h"

#ifdef CUDA_GPU

	#ifndef __NVCC__

		#define __host__
		#define __device__

		struct uint3
		{
			unsigned int x, y, z;
		};

		struct dim3
		{
			unsigned int x, y, z;
		#if defined(__cplusplus)
			__host__ __device__ dim3(unsigned int vx = 1, unsigned int vy = 1, unsigned int vz = 1) : x(vx), y(vy), z(vz) {}
			__host__ __device__ dim3(uint3 v) : x(v.x), y(v.y), z(v.z) {}
			__host__ __device__ operator uint3(void) { uint3 t; t.x = x; t.y = y; t.z = z; return t; }
		#endif /* __cplusplus */
		};

		namespace mgpu
		{
			// Stub class for modern gpu

			struct standard_context_t
			{
				standard_context_t(bool init)
				{}
			};
		}

	#else

		#ifndef __host__
		#define __host__
		#define __device__
		#endif

		#define CUDA_SAFE(cuda_call) \
		cuda_call; \
		{\
			cudaError_t e = cudaPeekAtLastError();\
			if (e != cudaSuccess)\
			{\
				std::string error = cudaGetErrorString(e);\
				std::cout << "Cuda Error in: " << __FILE__ << ":" << __LINE__ << " " << error << std::endl;\
			}\
		}

	#endif
#endif


#endif /* OPENFPM_DATA_SRC_UTIL_CUDA_UTIL_HPP_ */
