/*
 * Copyright 1993-2010 NVIDIA Corporation.  All rights reserved.
 *
 * Please refer to the NVIDIA end user license agreement (EULA) associated
 * with this source code for terms and conditions that govern your use of
 * this software. Any use, reproduction, disclosure, or distribution of
 * this software and related documentation outside the terms of the EULA
 * is strictly prohibited.
 *
 */

#ifndef _DEFINES_H_
#define _DEFINES_H_

#ifdef __cudaLegacy__         //legacy switch for cuda 4.2 and older (snow leopard)a
#define AL_CUDA_CHECK_ERRORS cutilSafeCall
#define AL_CUDA_INIT        shrQAStart(*pArgc, pArgv); \
                            cudaGLSetGLDevice( cutGetMaxGflopsDeviceId() ) 
#define AL_CUDA_DEVICE_RESET cutilDeviceReset()
#define AL_CUDA_EXIT shrQAFinishExit(*pArgc, (const char **)pArgv, QA_PASSED)
#else
#define AL_CUDA_CHECK_ERRORS checkCudaErrors
#define AL_CUDA_INIT findCudaGLDevice(*pArgc, (const char **)pArgv)
#define AL_CUDA_DEVICE_RESET cudaDeviceReset()
#endif

typedef unsigned int uint;
typedef unsigned char uchar;

// if SAMPLE_VOLUME is 0, an implicit dataset is generated. If 1, a voxelized
// dataset is loaded from file
#define SAMPLE_VOLUME 1

// Using shared to store computed vertices and normals during triangle generation
// improves performance
#define USE_SHARED 1

// The number of threads to use for triangle generation (limited by shared memory size)
#define NTHREADS 32

#define SKIP_EMPTY_VOXELS 1

#endif
