/*
 *  CPE 474 lab 0 - modern graphics test bed
 *  draws a partial cube using a VBO and IBO 
 *  glut/OpenGL/GLSL application   
 *  Uses glm and local matrix stack
 *  to handle matrix transforms for a view matrix, projection matrix and
 *  model transform matrix
 *
 *  zwood 9/12 
 *  Copyright 2012 Cal Poly. All rights reserved.
 *
 *****************************************************************************/

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

using namespace std;
using namespace glm;

GameConstants gc;

int const numSquares = 100;

GameObject cubes[numSquares];

//linked list of meshes 
GameObject *root;
GameObject *current;
GameObject *traverser;

GameObject *camera;

int numObjects = 0;

// Parameters
unsigned int WindowWidth = 1600, WindowHeight = 900;

// Variable Handles
GLuint aPosition;
GLuint aNormal;
GLuint uModelMatrix;
GLuint uNormalMatrix;
GLuint uViewMatrix;
GLuint uProjMatrix;
GLuint uColor;
GLuint uLightPos;
GLuint uLightColor;
GLuint uShininess;
GLuint uSpecStrength;
GLuint uCamTrans;

// Shader Handle
GLuint ShadeProg;

// Program Variables
float Accumulator;
glm::vec3 camLookAt = glm::vec3(0.f, 0.f, 0.f);
float camAlpha = 0;
float camBeta = 0;

glm::vec2 mousePos = glm::vec2(0.f);

int keyDown[128] = { 0 };

RenderingHelper ModelTrans;

void SetProjectionMatrix()
{
    glm::mat4 Projection = glm::perspective(80.0f, ((float) WindowWidth)/ ((float)WindowHeight), 0.1f, 100.f);
    safe_glUniformMatrix4fv(uProjMatrix, glm::value_ptr(Projection));
}

void SetView()
{
    glm::mat4 View = glm::lookAt(camera->trans, camLookAt, vec3(0.f, 1.f, 0.f));
    safe_glUniformMatrix4fv(uViewMatrix, glm::value_ptr(View));
}

void SetModel()
{
    safe_glUniformMatrix4fv(uModelMatrix, glm::value_ptr(ModelTrans.modelViewMatrix));
    safe_glUniformMatrix4fv(uNormalMatrix, glm::value_ptr(glm::transpose(glm::inverse(ModelTrans.modelViewMatrix))));
}

bool InstallShader(std::string const & vShaderName, std::string const & fShaderName)
{
    GLuint VS; // handles to shader object
    GLuint FS; // handles to frag shader object
    GLint vCompiled, fCompiled, linked; // status of shader

    VS = glCreateShader(GL_VERTEX_SHADER);
    FS = glCreateShader(GL_FRAGMENT_SHADER);

    // load the source
    char const * vSource = textFileRead(vShaderName);
    char const * fSource = textFileRead(fShaderName);
    glShaderSource(VS, 1, & vSource, NULL);
    glShaderSource(FS, 1, & fSource, NULL);

    // compile shader and print log
    glCompileShader(VS);
    printOpenGLError();
    glGetShaderiv(VS, GL_COMPILE_STATUS, & vCompiled);
    printShaderInfoLog(VS);

    // compile shader and print log
    glCompileShader(FS);
    printOpenGLError();
    glGetShaderiv(FS, GL_COMPILE_STATUS, & fCompiled);
    printShaderInfoLog(FS);

    if (! vCompiled || ! fCompiled)
    {
        std::cerr << "Error compiling either shader " << vShaderName << " or " << fShaderName << std::endl;
        return false;
    }

    // create a program object and attach the compiled shader
    ShadeProg = glCreateProgram();
    glAttachShader(ShadeProg, VS);
    glAttachShader(ShadeProg, FS);

    glLinkProgram(ShadeProg);

    // check shader status requires helper functions
    printOpenGLError();
    glGetProgramiv(ShadeProg, GL_LINK_STATUS, &linked);
    printProgramInfoLog(ShadeProg);

    glUseProgram(ShadeProg);

    // get handles to attribute data
    aPosition   = safe_glGetAttribLocation(ShadeProg, "aPosition");
    aNormal     = safe_glGetAttribLocation(ShadeProg, "aNormal");
    
    uColor          = safe_glGetUniformLocation(ShadeProg, "uColor");
    uProjMatrix     = safe_glGetUniformLocation(ShadeProg, "uProjMatrix");
    uViewMatrix     = safe_glGetUniformLocation(ShadeProg, "uViewMatrix");
    uModelMatrix    = safe_glGetUniformLocation(ShadeProg, "uModelMatrix");
    uNormalMatrix   = safe_glGetUniformLocation(ShadeProg, "uNormalMatrix");

    uLightPos       = safe_glGetUniformLocation(ShadeProg, "uLightPos");
    uLightColor     = safe_glGetUniformLocation(ShadeProg, "uLightColor");
    uShininess      = safe_glGetUniformLocation(ShadeProg, "uShininess");
    uSpecStrength   = safe_glGetUniformLocation(ShadeProg, "uSpecStrength");
    uCamTrans       = safe_glGetUniformLocation(ShadeProg, "uCamTrans");

    std::cout << "Successfully installed shader " << ShadeProg << std::endl;
    return true;
}

void Initialize()
{
    glClearColor(0.8f, 0.8f, 1.0f, 1.0f);

    glClearDepth(1.0f);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    ModelTrans.useModelViewMatrix();
    ModelTrans.loadIdentity();
}

void Draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /*glUseProgram(ShadeProg);

    SetProjectionMatrix();
    SetView();
    
    ModelTrans.loadIdentity();

    ModelTrans.pushMatrix();

        ModelTrans.rotate(Accumulator * 35.f, vec3(0, 1, 0));
        SetModel();

        safe_glEnableVertexAttribArray(aPosition);
        glBindBuffer(GL_ARRAY_BUFFER, Meshes[CurrentMesh]->PositionHandle);
        safe_glVertexAttribPointer(aPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

        safe_glEnableVertexAttribArray(aNormal);
        glBindBuffer(GL_ARRAY_BUFFER, Meshes[CurrentMesh]->NormalHandle);
        safe_glVertexAttribPointer(aNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Meshes[CurrentMesh]->IndexHandle);

        glUniform3f(uColor, 0.1f, 0.78f, 0.9f);

        glDrawElements(GL_TRIANGLES, Meshes[CurrentMesh]->IndexBufferLength, GL_UNSIGNED_SHORT, 0);

        safe_glDisableVertexAttribArray(aPosition);
        safe_glDisableVertexAttribArray(aNormal);

    ModelTrans.popMatrix();

    glUseProgram(0);*/

    for(int i = 0; i < 100; i++){
        cubes[i].draw(camera->trans, camLookAt,
            glm::vec3(0, 5, 0), glm::vec3(1,1,1), gc);        
    }

    current = root;

    while(current != NULL){
        current->draw(camera->trans, camLookAt, glm::vec3(0, 5, 0), glm::vec3(1,1,1), gc);
        current = current->next;
    }

    glfwSwapBuffers();
    printOpenGLError();
}

void Reshape(int width, int height)
{
    WindowWidth = width;
    WindowHeight = height;
    glViewport(0, 0, width, height);
}

void Keyboard(int key, int state)
{
    if (state == GLFW_PRESS)
        keyDown[key] = 1;
    else
        keyDown[key] = 0;
    switch (key)
    {
    
    // Camera up/down
    /*case 'W':
        CameraHeight += 0.1f;
        break;
    case 'S':
        CameraHeight -= 0.1f;
        break;*/

    // Toggle wireframe
    case 'N':
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDisable(GL_CULL_FACE);
        break;
    case 'M':
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glEnable(GL_CULL_FACE);
        break;

    // Quit program
    case 'Q':
        exit( EXIT_SUCCESS );
        break;
    
    }
}

void updateLookAt() {
    camLookAt = camera->trans - glm::vec3(
        cos(camAlpha)*cos(camBeta),
        sin(camAlpha),
        cos(camAlpha)*sin(camBeta));
}

void MouseMove(int x, int y) {
    glm::vec2 newPos = glm::vec2(x, y);
    //std::cout << "old " << mousePos.x << " " << mousePos.y << std::endl;
    //std::cout << "new " << x << " " << y << std::endl << std::endl;
    camAlpha += (float)(newPos.y - mousePos.y)/WindowHeight * 1;
    const float threshold = 3.14159f/2.f;
    if (camAlpha < -threshold)
        camAlpha = -threshold;
    else if (camAlpha > threshold) 
        camAlpha = threshold;
    camBeta += (float)(newPos.x - mousePos.x)/WindowWidth * 2;
    updateLookAt();
    glfwSetMousePos(WindowWidth/2, WindowHeight/2);
    mousePos = glm::vec2(WindowWidth/2, WindowHeight/2);
}

int main(int argc, char *argv[])
{
    // Initialize Global Variables

    if (!glfwInit()) {
        exit(EXIT_FAILURE);
    }
    if (!glfwOpenWindow(WindowWidth, WindowHeight, 0,0,0,0,32,0, GLFW_WINDOW)) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    // OpenGL Setup
    Initialize();
    getGLversion();

    // Shader Setup
    if (! InstallShader("mesh_vert.glsl", "mesh_frag.glsl"))
    {
        printf("Error installing shader!\n");
        return 1;
    }
    
    Mesh *tile = GeometryCreator::CreateCube(vec3(2.f, 0.1f, 2.f));
    Mesh *obstacle = GeometryCreator::CreateCube(vec3(1.f, 1.f, 1.f));

    int running = GL_TRUE;
    double oldTime = glfwGetTime();
    double curTime;

    gc.shader = ShadeProg;
    gc.aspectRatio = (float)WindowWidth/WindowHeight;
    gc.lightPos = glm::vec3(3.0f, 4.0f, 5.0f);
    gc.lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    gc.h_aPosition = aPosition;
    gc.h_aNormal = aNormal;
    gc.h_uColor = uColor;
    gc.h_uProjMatrix = uProjMatrix;
    gc.h_uViewMatrix = uViewMatrix;
    gc.h_uModelMatrix = uModelMatrix;
    gc.h_uNormalMatrix = uNormalMatrix;
    gc.h_uLightPos = uLightPos;
    gc.h_uLightColor = uLightColor;
    gc.h_uShininess = uShininess;
    gc.h_uSpecStrength = uSpecStrength;
    gc.h_uCamTrans = uCamTrans;

    srand(time(NULL));

    camera = new GameObject();
    camera->AABBmin = glm::vec3(-0.5f, -0.5f, -0.5f);
    camera->AABBmax = glm::vec3(0.5f, 0.5f, 0.5f);
    camera->dir = vec3(0.f);
    camera->speed = 0.f;
    camera->rotAxis = vec3(0.f, 1.f, 0.f);
    camera->rotSpeed = 0.f;
    camera->doTranslate(vec3(rand() % 20 - 10.5f, 0.5f, rand() % 20 - 10.5f));
    camera->scale = vec3(1.f);
    root = camera;
    camera->next = NULL;

    glfwSetKeyCallback(Keyboard);
    glfwSetMousePosCallback(MouseMove);

    for(int i = 0; i < 100; i++){
        cubes[i].VBO = tile->PositionHandle;
        cubes[i].IBO = tile->IndexHandle;
        cubes[i].IBOlen = tile->IndexBufferLength;
        cubes[i].NBO = tile->NormalHandle;
        cubes[i].dir = vec3(1.f, 0.f, 0.f);
        cubes[i].speed = 0.f;
        cubes[i].rotSpeed = 0.f;
        cubes[i].rotAxis = vec3(0.f, 1.f, 0.f);

        if((i / 10) % 2 == 0){
            if(i%2 == 0)
                cubes[i].color = vec3(0.3f, 0.3f, 0.3f);
            else
                cubes[i].color = vec3(1.f, 1.f, 1.f);
        }else if((i / 10) % 2 == 1){
            if(i%2 == 0)
                cubes[i].color = vec3(1.f, 1.f, 1.f);
            else
                cubes[i].color = vec3(0.3f, 0.3f, 0.3f);
        }

        cubes[i].shininess = 5;
        cubes[i].specStrength = 0.f;
        cubes[i].doTranslate(vec3(2.f*(i % 10) - 9.f, -0.2, 2.f * (i / 10) - 9.f));
        cubes[i].scale = glm::vec3(1.f);
    }

    while(running) {
        curTime = glfwGetTime();
        float dt = (float)(curTime - oldTime);
        Accumulator += dt;
        
        // Camera movement
        glm::vec3 up = glm::vec3(0, 1, 0);
        glm::vec3 forward = glm::normalize(camLookAt - camera->trans);
        glm::vec3 right = glm::normalize(glm::cross(forward, up));
        const float MOVE_SPEED = 8.f;
        if (keyDown['W'])
            camera->trans += MOVE_SPEED * forward * dt;
        if (keyDown['S'])
            camera->trans -= MOVE_SPEED * forward * dt;
        if (keyDown['D'])
            camera->trans += MOVE_SPEED * right * dt;
        if (keyDown['A'])
            camera->trans -= MOVE_SPEED * right * dt;
        if (camera->trans.y <= 0.f)
            camera->trans.y = 0.f;
        updateLookAt();

        for(int i = 0; i < 100; i++){
            cubes[i].update((float)(dt));
        }

        const double timeToAdd = 3.0;

        if(numObjects < 10){
            //add a new mesh to the list if enough time has passed
            if(Accumulator > timeToAdd){
                //add the mesh

                //TODO: add object info
                
                GameObject *temp = new GameObject();

                temp->VBO = obstacle->PositionHandle;
                temp->IBO = obstacle->IndexHandle;
                temp->IBOlen = obstacle->IndexBufferLength;
                temp->NBO = obstacle->NormalHandle;
                temp->AABBmin = glm::vec3(-0.5f, -0.5f, -0.5f);
                temp->AABBmax = glm::vec3(0.5f, 0.5f, 0.5f);
                float theta = rand() % (2*314159) / 10000.f;
                temp->dir = vec3(cos(theta), 0.f, sin(theta));
                temp->speed = 2.f;
                temp->rotAxis = vec3(0.f, 1.f, 0.f);
                temp->rotSpeed = 0.f;
                temp->color = vec3(0.2f, 0.2f, 1.0f);
                temp->shininess = 5;
                temp->specStrength = 1.0;
                //needs to be randomized
                temp->doTranslate(vec3(rand() % 20 - 10.5f, 0.3f, rand() % 20 -10.5f));
                temp->scale = glm::vec3(1.f);

                temp->next = NULL;

                current = root;

                while(current->next != NULL){
                    current = current->next;
                }
                current->next = temp;
                Accumulator = 0.0;

                numObjects++;
            }
        }

        //update all meshes
        current = root;

        while(current != NULL){
            current->update(dt);
            current = current->next;
        }

        static int numCollisions = 0;
        //check for collisions with camera
        traverser = root->next;
        while(traverser != NULL) {
            if(root->doesCollide(traverser)) {
                if (traverser->tag == 0)
                    numCollisions++;
                traverser->color = vec3(1.0f, 0.f, 0.f);
                traverser->speed = 0.f;
            }
            traverser = traverser->next;
        }

        //check for collisions b/n meshes
        current = root->next;
        while(current != NULL){
            traverser = current->next;

            while(traverser != NULL){
                if(current->doesCollide(traverser)){
                    current->dir = -current->dir;
                    traverser->dir = -traverser->dir;
                }
                traverser = traverser->next;
            }
            current = current->next;
        }

        oldTime = curTime;
        Draw();
    }

    glfwTerminate();

    return 0;
}
