
/*
 * Copyright 1993-2012 NVIDIA Corporation.  All rights reserved.
 *
 * Please refer to the NVIDIA end user license agreement (EULA) associated
 * with this source code for terms and conditions that govern your use of
 * this software. Any use, reproduction, disclosure, or distribution of
 * this software and related documentation outside the terms of the EULA
 * is strictly prohibited.
 *
 */

/* 
    Marching cubes

    This sample extracts a geometric isosurface from a volume dataset using
    the marching cubes algorithm. It uses the scan (prefix sum) function from
    the Thrust library to perform stream compaction.  Similar techniques can
    be used for other problems that require a variable-sized output per
    thread.

    For more information on marching cubes see:
    http://local.wasp.uwa.edu.au/~pbourke/geometry/polygonise/
    http://en.wikipedia.org/wiki/Marching_cubes

    Volume data courtesy:
    http://www9.informatik.uni-erlangen.de/External/vollib/

    For more information on the Thrust library
    http://code.google.com/p/thrust/

    The algorithm consists of several stages:

    1. Execute "classifyVoxel" kernel
    This evaluates the volume at the corners of each voxel and computes the
    number of vertices each voxel will generate.
    It is executed using one thread per voxel.
    It writes two arrays - voxelOccupied and voxelVertices to global memory.
    voxelOccupied is a flag indicating if the voxel is non-empty.

    2. Scan "voxelOccupied" array (using Thrust scan)
    Read back the total number of occupied voxels from GPU to CPU.
    This is the sum of the last value of the exclusive scan and the last
    input value.

    3. Execute "compactVoxels" kernel
    This compacts the voxelOccupied array to get rid of empty voxels.
    This allows us to run the complex "generateTriangles" kernel on only
    the occupied voxels.

    4. Scan voxelVertices array
    This gives the start address for the vertex data for each voxel.
    We read back the total number of vertices generated from GPU to CPU.

    Note that by using a custom scan function we could combine the above two
    scan operations above into a single operation.

    5. Execute "generateTriangles" kernel
    This runs only on the occupied voxels.
    It looks up the field values again and generates the triangle data,
    using the results of the scan to write the output to the correct addresses.
    The marching cubes look-up tables are stored in 1D textures.

    6. Render geometry
    Using number of vertices from readback.
*/

// std includes
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>


//#include "cutil_math.h"

#include <cutil_inline.h>    // includes cuda.h and cuda_runtime_api.h
#include <shrQATest.h>
#include <cuda_runtime_api.h>
#include <vector_types.h>
#include <vector_functions.h>


/* #include <GL/glew.h> */
/* #include <cutil_inline.h>    // includes cuda.h and cuda_runtime_api.h */
/* #include <cutil_gl_inline.h> */
/* #include <shrQATest.h> */
/* #include <cuda_runtime_api.h> */
/* #include <cuda_gl_interop.h> */
/* #include <rendercheck_gl.h> */
/* #include <vector_types.h> */
/* #include <vector_functions.h> */

#include "defines.h"
#include "graphics.hpp"
#include "io.hpp"

#define MAX_EPSILON_ERROR 5.0f
#define REFRESH_DELAY	  10 //ms
#define MAX(a,b) ((a > b) ? a : b)
#define EPSILON 5.0f
#define THRESHOLD 0.30f


//KERNEL FUNCTIONS (DEFINED IN marchingCubes_kernel.cu)
extern "C" void
launch_classifyVoxel( dim3 grid, dim3 threads, uint* voxelVerts, uint *voxelOccupied, uchar *volume,
					  uint3 gridSize, uint3 gridSizeShift, uint3 gridSizeMask, uint numVoxels,
					  float3 voxelSize, float isoValue);

extern "C" void 
launch_compactVoxels(dim3 grid, dim3 threads, uint *compactedVoxelArray, uint *voxelOccupied, 
					uint *voxelOccupiedScan, uint numVoxels);

extern "C" void
launch_generateTriangles(dim3 grid, dim3 threads,
						float4 *pos, float4 *norm, uint *compactedVoxelArray, uint *numVertsScanned,
						uint3 gridSize, uint3 gridSizeShift, uint3 gridSizeMask,
						float3 voxelSize, float isoValue, uint activeVoxels, uint maxVerts);

extern "C" void
launch_generateTriangles2(dim3 grid, dim3 threads,
						float4 *pos, float4 *norm, uint *compactedVoxelArray, uint *numVertsScanned, uchar *volume,
						uint3 gridSize, uint3 gridSizeShift, uint3 gridSizeMask,
						float3 voxelSize, float isoValue, uint activeVoxels, uint maxVerts);

extern "C" void allocateTextures(	uint **d_edgeTable, uint **d_triTable,  uint **d_numVertsTable );
extern "C" void bindVolumeTexture(uchar *d_volume);
extern "C" void ThrustScanWrapper(unsigned int* output, unsigned int* input, unsigned int numElements);


// Auto-Verification Code
/* const int frameCheckNumber = 4; */
/* int fpsCount = 0;        // FPS count for averaging */
/* int fpsLimit = 1;        // FPS limit for sampling */
/* int g_Index = 0; */
/* unsigned int frameCount = 0; */
/* unsigned int g_TotalErrors = 0; */
/* bool g_Verify = false; */
/* bool g_bQAReadback = false; */
/* bool g_bOpenGLQA   = false; */
/* bool g_bFBODisplay = false; */

/* int *pArgc = NULL; */
/* char **pArgv = NULL; */

// CheckFBO/BackBuffer class objects
/* CFrameBufferObject  *g_FrameBufferObject = NULL; */
/* CheckRender         *g_CheckRender       = NULL; */

/* template <class T> */
/* void dumpBuffer(T *d_buffer, int nelements, int size_element); */

/* template <class T> */
/* void dumpFile(T *d_buffer, int nelements, int size_element, const char *filename); */




struct MarchingCubesProgram {

    MarchingCubesProgram(){}

    ~MarchingCubesProgram(){
      cleanup();
      reset();
      exit();
    }

      bool bWireframe;
      float3 rotate;
      float3 translate;

    //member variables
      unsigned int timer;

      const char *volumeFilename ;

      uint3 gridSizeLog2;// = make_uint3(5, 5, 5);
      uint3 gridSizeShift;
      uint3 gridSize;
      uint3 gridSizeMask;

      float3 voxelSize;
      uint numVoxels    ;
      uint maxVerts     ;
      uint activeVoxels ;
      uint totalVerts   ;

      float isoValue	  ;
      float dIsoValue		;

      // device data
      GLuint posVbo, normalVbo;
      GLint  gl_Shader;
      struct cudaGraphicsResource *cuda_posvbo_resource, *cuda_normalvbo_resource; // handles OpenGL-CUDA exchange

      float4 *d_pos, *d_normal;

      uchar *d_volume ;
      uint *d_voxelVerts ;
      uint *d_voxelVertsScan ;
      uint *d_voxelOccupied ;
      uint *d_voxelOccupiedScan ;
      uint *d_compVoxelArray;

      // tables
      uint* d_numVertsTable;
      uint* d_edgeTable;
      uint* d_triTable;


      CheckRender         *g_CheckRender;


    ////////////////////////////////////////////////////////////////////////////////
    // initialize marching cubes
    ////////////////////////////////////////////////////////////////////////////////
    void init(int argc, char** argv)
    {

        timer = 0;
        volumeFilename = "Bucky.raw";

        numVoxels    = 0;
        maxVerts     = 0;
        activeVoxels = 0;
        totalVerts   = 0;

        isoValue		= 0.2f;
        dIsoValue		= 0.005f;

        d_pos = 0; d_normal = 0; d_volume = 0; d_voxelVerts = 0; d_voxelVertsScan=0;
        d_voxelOccupied=0;  d_voxelOccupiedScan=0;  d_compVoxelArray=0;

        d_numVertsTable=0; d_edgeTable=0; d_triTable=0;

        gridSizeLog2 = make_uint3(5, 5, 5);
        gridSize = make_uint3(1<<gridSizeLog2.x, 1<<gridSizeLog2.y, 1<<gridSizeLog2.z);
        gridSizeMask = make_uint3(gridSize.x-1, gridSize.y-1, gridSize.z-1);
        gridSizeShift = make_uint3(0, gridSizeLog2.x, gridSizeLog2.x+gridSizeLog2.y);

        numVoxels = gridSize.x*gridSize.y*gridSize.z;
        voxelSize = make_float3(2.0f / gridSize.x, 2.0f / gridSize.y, 2.0f / gridSize.z);
        maxVerts = gridSize.x*gridSize.y*100;

        bWireframe = false;
        rotate = make_float3(0.0, 0.0, 0.0);
        translate= make_float3(0.0, 0.0, -3.0);

        g_CheckRender = NULL;


        printf("grid: %d x %d x %d = %d voxels\n", gridSize.x, gridSize.y, gridSize.z, numVoxels);
        printf("max verts = %d\n", maxVerts);

#if SAMPLE_VOLUME
        // load volume data
        char* path = cutFindFilePath(volumeFilename, argv[0]);
        if (path == NULL) {
            fprintf(stderr, "Error finding file '%s'\n", volumeFilename);
            cutilDeviceReset();
            shrQAFinishExit(argc, (const char **)argv, QA_FAILED);
        }

        int size = gridSize.x*gridSize.y*gridSize.z*sizeof(uchar);
        uchar *volume = loadRawFile(path, size);
        cutilSafeCall(cudaMalloc((void**) &d_volume, size));
        cutilSafeCall(cudaMemcpy(d_volume, volume, size, cudaMemcpyHostToDevice) );
        free(volume);

      bindVolumeTexture(d_volume);
#endif

        // create VBOs
        createVBO(&posVbo, maxVerts*sizeof(float)*4);
        cutilSafeCall(cudaGraphicsGLRegisterBuffer(&cuda_posvbo_resource, posVbo, 
        cudaGraphicsMapFlagsWriteDiscard));

        createVBO(&normalVbo, maxVerts*sizeof(float)*4);
        cutilSafeCall(cudaGraphicsGLRegisterBuffer(&cuda_normalvbo_resource, normalVbo, 
        cudaGraphicsMapFlagsWriteDiscard));

        // allocate textures
        allocateTextures(	&d_edgeTable, &d_triTable, &d_numVertsTable );

        // allocate device memory
        unsigned int memSize = sizeof(uint) * numVoxels;
        cutilSafeCall(cudaMalloc((void**) &d_voxelVerts,            memSize));
        cutilSafeCall(cudaMalloc((void**) &d_voxelVertsScan,        memSize));
        cutilSafeCall(cudaMalloc((void**) &d_voxelOccupied,         memSize));
        cutilSafeCall(cudaMalloc((void**) &d_voxelOccupiedScan,     memSize));
        cutilSafeCall(cudaMalloc((void**) &d_compVoxelArray,   memSize));
    }

    
    void cleanup()
    {
        cutilCheckError( cutDeleteTimer( timer ));

        deleteVBO(&posVbo,    &cuda_posvbo_resource);
        deleteVBO(&normalVbo, &cuda_normalvbo_resource);

        cutilSafeCall(cudaFree(d_edgeTable));
        cutilSafeCall(cudaFree(d_triTable));
        cutilSafeCall(cudaFree(d_numVertsTable));

        cutilSafeCall(cudaFree(d_voxelVerts));
        cutilSafeCall(cudaFree(d_voxelVertsScan));
        cutilSafeCall(cudaFree(d_voxelOccupied));
        cutilSafeCall(cudaFree(d_voxelOccupiedScan));
        cutilSafeCall(cudaFree(d_compVoxelArray));

        if (d_volume) cutilSafeCall(cudaFree(d_volume));
    }


    void
    run(int argc, char** argv)
    {
       printf("MarchingCubes ");
       
       shrQAStart(argc, argv); 
       cudaGLSetGLDevice( cutGetMaxGflopsDeviceId() ); 
       init(argc, argv);
       cutilCheckError( cutCreateTimer( &timer));

      //now run GL loop
   }

    void start(int argc, char** argv){ shrQAStart(argc, argv); }
    void reset(){ cutilDeviceReset(); }
    void exit(int argc=0, char** argv=NULL) { shrQAFinishExit(argc, (const char **)argv, QA_PASSED); }


    //DEBUGGING      
#define DEBUG_BUFFERS 0

    /* template <class T> */
    /* void dumpBuffer(T *d_buffer, int nelements, int size_element) */
    /* { */
    /*     uint bytes = nelements * size_element; */
    /*     T *h_buffer = (T *) malloc(bytes); */
    /*     cutilSafeCall( cudaMemcpy(h_buffer, d_buffer, bytes, cudaMemcpyDeviceToHost) ); */
    /*     for(int i=0; i<nelements; i++) { */
    /*         printf("%d: %u\n", i, h_buffer[i]); */
    /*     } */
    /*     printf("\n"); */
    /*     free(h_buffer); */
    /* } */

    /* template <class T> */
    /* void dumpFile(T *d_buffer, int nelements, int size_element, const char *filename) */
    /* { */
    /*     cutilSafeCall( cudaMemcpy( (T *)g_CheckRender->imageData(), (T *)d_buffer, nelements*size_element, cudaMemcpyDeviceToHost) ); */
    /*     g_CheckRender->dumpBin((unsigned char *)g_CheckRender->imageData(), nelements*size_element, filename); */
    /* } */

    ////////////////////////////////////////////////////////////////////////////////
    //! Run the Cuda part of the computation
    ////////////////////////////////////////////////////////////////////////////////
      void computeIsosurface()
      {
          int threads = 128;
          dim3 grid(numVoxels / threads, 1, 1);
          // get around maximum grid size of 65535 in each dimension
          if (grid.x > 65535) {
              grid.y = grid.x / 32768;
              grid.x = 32768;
          }

          // calculate number of vertices need per voxel
          launch_classifyVoxel(grid, threads, 
                  d_voxelVerts, d_voxelOccupied, d_volume, 
                  gridSize, gridSizeShift, gridSizeMask, 
                  numVoxels, voxelSize, isoValue);
#if DEBUG_BUFFERS
          printf("voxelVerts:\n");
          dumpBuffer(d_voxelVerts, numVoxels, sizeof(uint));
#endif

#if SKIP_EMPTY_VOXELS
          // scan voxel occupied array
          ThrustScanWrapper(d_voxelOccupiedScan, d_voxelOccupied, numVoxels);

#if DEBUG_BUFFERS
          printf("voxelOccupiedScan:\n");
          dumpBuffer(d_voxelOccupiedScan, numVoxels, sizeof(uint));
#endif

          // read back values to calculate total number of non-empty voxels
          // since we are using an exclusive scan, the total is the last value of
          // the scan result plus the last value in the input array
          {
              uint lastElement, lastScanElement;
              cutilSafeCall(cudaMemcpy((void *) &lastElement, 
                             (void *) (d_voxelOccupied + numVoxels-1), 
                             sizeof(uint), cudaMemcpyDeviceToHost));
              cutilSafeCall(cudaMemcpy((void *) &lastScanElement, 
                             (void *) (d_voxelOccupiedScan + numVoxels-1), 
                             sizeof(uint), cudaMemcpyDeviceToHost));
              activeVoxels = lastElement + lastScanElement;
          }

          if (activeVoxels==0) {
              // return if there are no full voxels
              totalVerts = 0;
              return;
          }

          // compact voxel index array
          launch_compactVoxels(grid, threads, d_compVoxelArray, d_voxelOccupied, d_voxelOccupiedScan, numVoxels);
          cutilCheckMsg("compactVoxels failed");

#endif // SKIP_EMPTY_VOXELS

          // scan voxel vertex count array
          ThrustScanWrapper(d_voxelVertsScan, d_voxelVerts, numVoxels);

#if DEBUG_BUFFERS
          printf("voxelVertsScan:\n");
          dumpBuffer(d_voxelVertsScan, numVoxels, sizeof(uint));
#endif

          // readback total number of vertices
          {
              uint lastElement, lastScanElement;
              cutilSafeCall(cudaMemcpy((void *) &lastElement, 
                             (void *) (d_voxelVerts + numVoxels-1), 
                             sizeof(uint), cudaMemcpyDeviceToHost));
              cutilSafeCall(cudaMemcpy((void *) &lastScanElement, 
                             (void *) (d_voxelVertsScan + numVoxels-1), 
                             sizeof(uint), cudaMemcpyDeviceToHost));
              totalVerts = lastElement + lastScanElement;
          }

          // generate triangles, writing to vertex buffers
          size_t num_bytes;
            // DEPRECATED: cutilSafeCall(cudaGLMapBufferObject((void**)&d_pos, posVbo));
            cutilSafeCall(cudaGraphicsMapResources(1, &cuda_posvbo_resource, 0));
            cutilSafeCall(cudaGraphicsResourceGetMappedPointer((void**)&d_pos, &num_bytes, cuda_posvbo_resource));

            // DEPRECATED: cutilSafeCall(cudaGLMapBufferObject((void**)&d_normal, normalVbo));
            cutilSafeCall(cudaGraphicsMapResources(1, &cuda_normalvbo_resource, 0));
            cutilSafeCall(cudaGraphicsResourceGetMappedPointer((void**)&d_normal, &num_bytes, cuda_normalvbo_resource));

#if SKIP_EMPTY_VOXELS
          dim3 grid2((int) ceil(activeVoxels / (float) NTHREADS), 1, 1);
#else
          dim3 grid2((int) ceil(numVoxels / (float) NTHREADS), 1, 1);
#endif
          while(grid2.x > 65535) {
              grid2.x/=2;
              grid2.y*=2;
          }
#if SAMPLE_VOLUME
          launch_generateTriangles2(grid2, NTHREADS, d_pos, d_normal, 
                                                  d_compVoxelArray, 
                                                  d_voxelVertsScan, d_volume, 
                                                  gridSize, gridSizeShift, gridSizeMask, 
                                                  voxelSize, isoValue, activeVoxels, 
                                                  maxVerts);
#else
          launch_generateTriangles(grid2, NTHREADS, d_pos, d_normal, 
                                                 d_compVoxelArray, 
                                                 d_voxelVertsScan, 
                                                 gridSize, gridSizeShift, gridSizeMask, 
                                                 voxelSize, isoValue, activeVoxels, 
                                                 maxVerts);
#endif

          cutilSafeCall(cudaGraphicsUnmapResources(1, &cuda_normalvbo_resource, 0));
          cutilSafeCall(cudaGraphicsUnmapResources(1, &cuda_posvbo_resource, 0));
      }


      ////////////////////////////////////////////////////////////////////////////////
      //! Display callback
      ////////////////////////////////////////////////////////////////////////////////
      void onFrame()
      {
          //cutilCheckError(cutStartTimer(timer));  
          computeIsosurface();

          // Common display code path
        {
          glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

          // set view matrix
          glMatrixMode(GL_MODELVIEW);
          glLoadIdentity();
          glTranslatef(translate.x, translate.y, translate.z);
          glRotatef(rotate.x, 1.0, 0.0, 0.0);
          glRotatef(rotate.y, 0.0, 1.0, 0.0);

          glPolygonMode(GL_FRONT_AND_BACK, bWireframe? GL_LINE : GL_FILL);
          //if (lighting) {
            glEnable(GL_LIGHTING);
          //}

          // render
          //if (render) {
            glPushMatrix();
            glRotatef(180.0, 0.0, 1.0, 0.0);
            glRotatef(90.0, 1.0, 0.0, 0.0);
            renderIsosurface(totalVerts);
            glPopMatrix();
          //}

          glDisable(GL_LIGHTING);
        } 

         // cutilCheckError(cutStopTimer(timer));  

         // computeFPS();

       }


};
