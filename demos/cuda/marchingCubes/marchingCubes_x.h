/*
 * =====================================================================================
 *
 *       Filename:  marchingCubes_x.h
 *
 *    Description:  header only . . .
 *
 *        Version:  1.0
 *        Created:  08/13/2014 16:23:55
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail -> wolftype
 *   Organization:  
 *
 * =====================================================================================
 */


#ifndef  marchingCubes_x_INC
#define  marchingCubes_x_INC

// std includes
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>


#include <GL/glew.h>
#include <vector_types.h>
#include "defines.h"

/* struct tuint3  { uint x, y, z; }; */
/* struct tfloat3 { float x,y,z; }; */
/* struct tfloat4 { float x,y,z,w; }; */

struct MarchingCubesProgram{

    MarchingCubesProgram(int argc, char **argv);
    ~MarchingCubesProgram();

    void init();

    void cleanup();

    void run();
    void start();
    void reset();
    void exit();

     ////////////////////////////////////////////////////////////////////////////////
     //! Run the Cuda part of the computation
     ////////////////////////////////////////////////////////////////////////////////
      void computeIsosurface();

      ////////////////////////////////////////////////////////////////////////////////
      //! Display callback
      ////////////////////////////////////////////////////////////////////////////////
      void onDraw();
      
      int *pArgc;
      char **pArgv;

      bool bWireframe;
      float3 rotate;
      float3 translate;

     //member variables
      unsigned int timer;

      char *volumeFilename ;

      uint3 gridSizeLog2;// = make_uint3(5, 5, 5);
      uint3 gridSizeShift;
      uint3 gridSize;
      uint3 gridSizeMask;

      float3 voxelSize;
      float4 *d_pos, *d_normal;
      
      uint numVoxels;
      uint maxVerts;
      uint activeVoxels;
      uint totalVerts;

      float isoValue;
      float dIsoValue;

      // device data
      GLuint posVbo, normalVbo;
      GLint  gl_Shader;

      // handles OpenGL-CUDA exchange
      struct cudaGraphicsResource *cuda_posvbo_resource, *cuda_normalvbo_resource; 

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

};


#endif   /* ----- #ifndef marchingCubes_x_INC  ----- */
