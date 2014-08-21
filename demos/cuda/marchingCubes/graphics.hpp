/*
 * =====================================================================================
 *
 *       Filename:  graphics.hpp
 *
 *    Description:  vbos and such
 *
 *        Version:  1.0
 *        Created:  08/08/2014 16:54:36
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail -> wolftype
 *   Organization:  
 *
 * =====================================================================================
 */

#include <GL/glew.h>

#include <cuda_runtime.h>
#include <cuda_gl_interop.h>
#include <rendercheck_gl.h>

#ifdef __cudaLegacy__         //legacy switch for cuda 4.2 and older (snow leopard)
#include <cutil_inline.h>    // includes cuda.h and cuda_runtime_api.h
#include <cutil_gl_inline.h>
#else
#include <helper_cuda.h>
#include <helper_cuda_gl.h>
#endif




// device data
//GLuint posVbo, normalVbo;
//GLint  gl_Shader;


// shader for displaying floating-point texture
static const char *shader_code = 
"!!ARBfp1.0\n"
"TEX result.color, fragment.texcoord, texture[0], 2D; \n"
"END";

GLuint compileASMShader(GLenum program_type, const char *code)
{
    GLuint program_id;
    glGenProgramsARB(1, &program_id);
    glBindProgramARB(program_type, program_id);
    glProgramStringARB(program_type, GL_PROGRAM_FORMAT_ASCII_ARB, (GLsizei) strlen(code), (GLubyte *) code);

    GLint error_pos;
    glGetIntegerv(GL_PROGRAM_ERROR_POSITION_ARB, &error_pos);
    if (error_pos != -1) {
        const GLubyte *error_string;
        error_string = glGetString(GL_PROGRAM_ERROR_STRING_ARB);
        fprintf(stderr, "Program error at position: %d\n%s\n", (int)error_pos, error_string);
        return 0;
    }
    return program_id;
}

////////////////////////////////////////////////////////////////////////////////
//! Create VBO
////////////////////////////////////////////////////////////////////////////////
void
createVBO(GLuint* vbo, unsigned int size)
{
    // create buffer object
    printf("glGenBuffers\n");
    glGenBuffers(1, vbo);
    printf("glBindBuffer\n");
    glBindBuffer(GL_ARRAY_BUFFER, *vbo);

    // initialize buffer object
    printf("glBufferData\n");
    glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

   // glutReportErrors();
}

////////////////////////////////////////////////////////////////////////////////
//! Delete VBO
////////////////////////////////////////////////////////////////////////////////
void
deleteVBO(GLuint* vbo, struct cudaGraphicsResource **cuda_resource)
{
    glBindBuffer(1, *vbo);
    glDeleteBuffers(1, vbo);
	//DEPRECATED: cutilSafeCall(cudaGLUnregisterBufferObject(*vbo));    
	cudaGraphicsUnregisterResource(*cuda_resource);

    *vbo = 0;
}

////////////////////////////////////////////////////////////////////////////////
// Render isosurface geometry from the vertex buffers
////////////////////////////////////////////////////////////////////////////////
void renderIsosurface( int numVerts, GLuint pvbo, GLuint nvbo )
{
    glBindBuffer(GL_ARRAY_BUFFER, pvbo);
    glVertexPointer(4, GL_FLOAT, 0, 0);
    glEnableClientState(GL_VERTEX_ARRAY);

    glBindBufferARB(GL_ARRAY_BUFFER_ARB, nvbo);
    glNormalPointer(GL_FLOAT, sizeof(float)*4, 0);
    glEnableClientState(GL_NORMAL_ARRAY);

    glColor3f(1.0, 0.0, 0.0);
    glDrawArrays(GL_TRIANGLES, 0, numVerts);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void setDefaults(){
      // default initialization
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    // good old-fashioned fixed function lighting
    float black[]    = { 0.0f, 0.0f, 0.0f, 1.0f };
    float white[]    = { 1.0f, 1.0f, 1.0f, 1.0f };
    float ambient[]  = { 0.1f, 0.1f, 0.1f, 1.0f };
    float diffuse[]  = { 0.9f, 0.9f, 0.9f, 1.0f };
    float lightPos[] = { 0.0f, 0.0f, 1.0f, 0.0f };

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, black);

    glLightfv(GL_LIGHT0, GL_AMBIENT, white);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
    glLightfv(GL_LIGHT0, GL_SPECULAR, white);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, black);

    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);

}

/*-----------------------------------------------------------------------------
 *  FPS
 *-----------------------------------------------------------------------------*/
/* void computeFPS() */
/* { */
/*     frameCount++; */
/*     fpsCount++; */
/*     if (fpsCount == fpsLimit-1) { */
/*         g_Verify = true; */
/*     } */
/*     if (fpsCount == fpsLimit) { */
/*         char fps[256]; */
/*         float ifps = 1.f / (cutGetAverageTimerValue(timer) / 1000.f); */
/*         sprintf(fps, "%sCUDA Marching Cubes: %3.1f fps", */ 
/*                 ((g_CheckRender && g_CheckRender->IsQAReadback()) ? "AutoTest: " : ""), ifps); */  

/*         glutSetWindowTitle(fps); */
/*         fpsCount = 0; */ 
/*         if (g_CheckRender && !g_CheckRender->IsQAReadback()) fpsLimit = (int)MAX(ifps, 1.f); */

/*         cutilCheckError(cutResetTimer(timer)); */  

/*         AutoQATest(); */
/*     } */
/* } */

