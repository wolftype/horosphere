/*
 * =====================================================================================
 *
 *       Filename:  interface.hpp
 *
 *    Description:  keyboard and mouse
 *
 *        Version:  1.0
 *        Created:  08/08/2014 16:55:45
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail -> wolftype
 *   Organization:  
 *
 * =====================================================================================
 */

// mouse controls
int mouse_old_x, mouse_old_y;
int mouse_buttons = 0;
float3 rotate = make_float3(0.0, 0.0, 0.0);
float3 translate= make_float3(0.0, 0.0, -3.0);

// toggles
bool wireframe = false;
bool animate   = true;
bool lighting  = true;
bool render    = true;
bool compute   = true;


void initMenus()
{
    glutCreateMenu(mainMenu);
    glutAddMenuEntry("Toggle animation [ ]", ' ');
    glutAddMenuEntry("Increment isovalue [+]", '+');
    glutAddMenuEntry("Decrement isovalue [-]", '-');
    glutAddMenuEntry("Toggle computation [c]", 'c');
    glutAddMenuEntry("Toggle rendering [r]", 'r');
    glutAddMenuEntry("Toggle lighting [l]", 'l');
    glutAddMenuEntry("Toggle wireframe [w]", 'w');
    glutAddMenuEntry("Quit (esc)", '\033');
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}


////////////////////////////////////////////////////////////////////////////////
//! Keyboard events handler
////////////////////////////////////////////////////////////////////////////////
void
keyboard(unsigned char key, int /*x*/, int /*y*/)
{
    switch(key) {
    case(27) :
        cleanup();
        cutilDeviceReset();
        exit(0);
    case '=':
        isoValue += 0.01f;
        break;
    case '-':
        isoValue -= 0.01f;
        break;
    case '+':
        isoValue += 0.1f;
        break;
    case '_':
        isoValue -= 0.1f;
        break;
    case 'w':
        wireframe = !wireframe;
        break;
    case ' ':
        animate = !animate;
        break;
    case 'l':
        lighting = !lighting;
        break;
    case 'r':
        render = !render;
        break;
    case 'c':
        compute = !compute;
        break;
    }

    printf("isoValue = %f\n", isoValue);
    printf("voxels = %d\n", activeVoxels);
    printf("verts = %d\n", totalVerts);
    printf("occupancy: %d / %d = %.2f%%\n", 
           activeVoxels, numVoxels, activeVoxels*100.0f / (float) numVoxels);

    if (!compute) {
        computeIsosurface();        
    }
}

////////////////////////////////////////////////////////////////////////////////
//! Mouse event handlers
////////////////////////////////////////////////////////////////////////////////
void
mouse(int button, int state, int x, int y)
{
    if (state == GLUT_DOWN) {
        mouse_buttons |= 1<<button;
    } else if (state == GLUT_UP) {
        mouse_buttons = 0;
    }

    mouse_old_x = x;
    mouse_old_y = y;
}

void motion(int x, int y)
{
    float dx = (float)(x - mouse_old_x);
    float dy = (float)(y - mouse_old_y);

    if (mouse_buttons==1) {
        rotate.x += dy * 0.2f;
        rotate.y += dx * 0.2f;
    } else if (mouse_buttons==2) {
        translate.x += dx * 0.01f;
        translate.y -= dy * 0.01f;
    } else if (mouse_buttons==3) {
        translate.z += dy * 0.01f;
    }

    mouse_old_x = x;
    mouse_old_y = y;
    glutPostRedisplay();
}

void idle()
{
    animation();
    glutPostRedisplay();
}

void reshape(int w, int h)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (float) w / (float) h, 0.1, 10.0);

    glMatrixMode(GL_MODELVIEW);
    glViewport(0, 0, w, h);
}

void mainMenu(int i)
{
    keyboard((unsigned char) i, 0, 0);
}

