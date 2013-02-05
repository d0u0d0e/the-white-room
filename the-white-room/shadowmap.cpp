// shadow mapping tutorial
// http://www.paulsprojects.net/tutorials/smt/smt.html
#include <GL/glfw.h>

#include <iostream>
#include <sstream>
#include <string>
#include <math.h>
#include <time.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "GLSL_helper.h"
#include "MStackHelp.h"

#include "GeometryCreator.h"
#include "GameObject.h"

#include "VFCull.h"

void main() {
    
    //initializing data
    VECTOR3D cameraPosition(-2.5f, 3.5f, -2.5f);
    VECTOR3D lightPosition(2.0f, 3.0f, -2.0f);
    
    const int shadowMapSize = 512;
    
    GLuint shadowMapTexture;
    
    int windowWidth, windowHeight;
    
    GLfloat white[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat black[] = {0.0f, 0.0f, 0.0f, 1.0f};
    
    MATRIX4X4 lightProjectionMatrix, lightViewMatrix;
    MATRIX4X4 cameraProjectionMatrix, cameraViewMatrix;
    
    glLoadIdentity();
    gluPerspective(45.0f, (float)windowWidth/windowHeight, 1.0f, 100.0f);
    glGetFloatv(GL_MODELVIEW_MATRIX, cameraProjectionMatrix);

    glLoadIdentity();
    gluLookAt(cameraPosition.x, cameraPosition.y, cameraPosition.z,
    0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f);
    glGetFloatv(GL_MODELVIEW_MATRIX, cameraViewMatrix);
    
    glLoadIdentity();
    gluPerspective(45.0f, 1.0f, 2.0f, 8.0f);
    glGetFloatv(GL_MODELVIEW_MATRIX, lightProjectionMatrix);
    
    glLoadIdentity();
    gluLookAt(lightPosition.x, lightPosition.y, lightPosition.z,
               0.0f, 0.0f, 0.0f,
               0.0f, 1.0f, 0.0f);
    glGetFloatv(GL_MODELVIEW_MATRIX, lightViewMatrix);
    
    
    //create shadow map texture
    glGenTextures(1, &shadowMapTexture);
    glBindTexture(GL_TEXTURE_2D, shadowMapTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowMapSize, shadowMapSize, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    
// first pass (draw scene from light's POV): clear color/depth buffers and set matrices for light. viewport size = shadow map size. 
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

glMatrixMode(GL_PROJECTION);
glLoadMatrixf(lightProjectionMatrix);

glMatrixMode(GL_MODELVIEW);
glLoadMatrixf(lightViewMatrix);

glViewport(0, 0, shadowMapSize, shadowMapSize);

//draw back faces to shadowmap
glCullFace(GL_FRONT);

glShadeModel(GL_FLAT);
glColorMask(0, 0, 0, );

//draw scene

//read depth buffer into shadow map texture
glBindTexture(GL_TEXTURE_2D, shadowMapTexture);
glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, shadowMapSize, shadowMapSize);

//restore states
glCullFace(GL_BACK);
glShadeModel(GL_SMOOTH);
glColorMask(1, 1, 1, 1);



//second pass (draw scene from camera's POV): light set to brightness of shadowed areas. clear depth buffer, set up matrices to camera's POV, use viewport that covers whole window
glClear(GL_DEPTH_BUFFER_BIT);
glMatrixMode(GL_PROJECTION);
glLoadMatrixf(cameraProjectionMatrix);

glMatrixMode(GL_MODELVIEW);
glLoadMatrixf(cameraViewMatrix);

glViewport(0, 0, windowWidth, windowHeight);

//dim light to represent shadow
glLightfv(GL_LIGHT1, GL_POSITION, VECTOR4D(lightPosition));
glLightfv(GL_LIGHT1, GL_AMBIENT, white*0.2f);
glLightfv(GL_LIGHT1, GL_DIFFUSE, white*0.2f);
glLightfv(GL_LIGHT1, GL_SPECULAR, black);
glEnable(GL_LIGHT1);
glEnable(GL_LIGHTING);

//draw scene



//third pass(shadow calculations): bright light w/ full brightness to illuminate unshadowed objects
glLightfv(GL_LIGHT1, GL_DIFFUSE, white);
glLightfv(GL_LIGHT1, GL_SPECULAR, white);

//texture matrix (eye -> light's clip space)
//using column major
static MATRIX4X4 biasMatrix
(0.5f, 0.0f, 0.0f, 0.0f,
0.0f, 0.5f, 0.0f, 0.0f,
0.0f, 0.0f, 0.5f, 0.0f,
0.5f, 0.5f, 0.5f, 1.0f);

//bias from [-1, 1] to [0,1]
Matrix4x4 *textureMatrix = biasMatrix*lightProjectionMatrix*lightViewMatrix;

//texture coordinate generation
glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
glTexGenfv(GL_S, GL_EYE_PLANE, textureMatrix.GetRow(0));
glEnable(GL_TEXTURE_GEN_S);

glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
glTexGenfv(GL_S, GL_EYE_PLANE, textureMatrix.GetRow(1));
glEnable(GL_TEXTURE_GEN_S);

glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
glTexGenfv(GL_S, GL_EYE_PLANE, textureMatrix.GetRow(2));
glEnable(GL_TEXTURE_GEN_S);

glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
glTexGenfv(GL_S, GL_EYE_PLANE, textureMatrix.GetRow(3));
glEnable(GL_TEXTURE_GEN_S);

//bind & enable shadow map texture
glBindTexture(GL_TEXTURE_2D, shadowMapTexture);
glEnable(GL_TEXTURE_2D);

//shadow comparison
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE);

//r <= texture means not in shadow (true)
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC_ARB, GL_LEQUAL);

//intensity result
glTexParameteri (GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE_ARB, GL_INTENSITY);

//alpha test to discard false comparisons (alpha < 0.99)
glAlphaFunc(GL_GEQUAL, 0.99f);
glEnable(GL_ALPHA_TEST);

//draw scene

//disable textures and texgen
glDisable(GL_TEXTURE_2D);
glDisable(GL_TEXTURE_GEN_S);
glDisable(GL_TEXTURE_GEN_T);
glDisable(GL_TEXTURE_GEN_R);
glDisable(GL_TEXTURE_GEN_Q);

//restore other states
glDisable(GL_LIGHTING);
glDisable(GL_ALPHA_TEST);

//reset matrices
glMatrixMode(GL_PROJECTION);
glPopMatrix();
glMatrixMode(GL_MODELVIEW);
glPopMatrix();

glFinish();
glutSwapBuffers();
glutPostRedisplay();


}