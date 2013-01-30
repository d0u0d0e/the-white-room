#include <GL/gl.h>
#include <stdio.h>
#include "GLSL_helper.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "GameObject.h"

GameObject::GameObject(GLuint VBO, GLuint IBO, int IBOlen) {
    this->IBO = IBO;
    this->IBOlen = IBOlen;
    this->VBO = VBO;
}

GameObject::GameObject() {
    this->IBO = NULL;
    dir = glm::vec3(0.f);
    speed = 0.f;
    rotAxis = glm::vec3(0.f, 1.f, 0.f);
    rotSpeed = 0.f;
    AABBmin = glm::vec3(-1.f);
    AABBmax = glm::vec3(1.f);
    IBO = -1;
    IBOlen = -1;
    VBO = -1;
    NBO = -1;
    glm::vec3 scale = glm::vec3(1.f);
    glm::mat4 rotate = glm::mat4();
    glm::vec3 trans = glm::vec3(0.f);
    tag = 0;
}

GameObject::~GameObject() {}

void GameObject::draw(glm::vec3 cameraPos, glm::vec3 lookAt,
    glm::vec3 lightPos, glm::vec3 lightColor, GameConstants gc) {
    if (VBO == -1 || IBO == -1 || IBOlen <= 0 || NBO == -1)
        return;

    glUseProgram(gc.shader);

    //TODO Set matrix stuff
    glm::mat4 projection = glm::perspective(80.0f, gc.aspectRatio, 0.1f, 100.f);
    safe_glUniformMatrix4fv(gc.h_uProjMatrix, glm::value_ptr(projection));

    glm::mat4 view = glm::lookAt(cameraPos, lookAt, glm::vec3(0.f, 1.f, 0.f));
    safe_glUniformMatrix4fv(gc.h_uViewMatrix, glm::value_ptr(view));

    glm::mat4 transMat = glm::translate(glm::mat4(), trans);
    glm::mat4 scaleMat = glm::scale(glm::mat4(), scale);

    //glm::mat4 model = transMat * rotate * scaleMat;
    glm::mat4 model = transMat * rotate * scaleMat;
    safe_glUniformMatrix4fv(gc.h_uModelMatrix, glm::value_ptr(model));
    safe_glUniformMatrix4fv(gc.h_uNormalMatrix, glm::value_ptr(glm::transpose(glm::inverse(model))));
    
    //Do transformations
    safe_glEnableVertexAttribArray(gc.h_aPosition);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    safe_glVertexAttribPointer(gc.h_aPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

    safe_glEnableVertexAttribArray(gc.h_aNormal);
    glBindBuffer(GL_ARRAY_BUFFER, NBO);
    safe_glVertexAttribPointer(gc.h_aNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    
    glUniform3f(gc.h_uColor, color.x, color.y, color.z);
    glUniform1f(gc.h_uShininess, shininess);
    glUniform1f(gc.h_uSpecStrength, specStrength);
    glUniform3f(gc.h_uLightPos, lightPos.x, lightPos.y, lightPos.z);
    glUniform3f(gc.h_uLightColor, lightColor.x, lightColor.y, lightColor.z);
    glUniform3f(gc.h_uCamTrans, cameraPos.x, cameraPos.y, cameraPos.z);

    glDrawElements(GL_TRIANGLES, IBOlen, GL_UNSIGNED_SHORT, 0);

    safe_glDisableVertexAttribArray(gc.h_aNormal);
    safe_glDisableVertexAttribArray(gc.h_aPosition);
    glUseProgram(0);
}

void GameObject::update(float dt) {
    doTranslate(glm::vec3(dt * speed * dir));
    if (trans.x + AABBmin.x < -10) {
        trans.x = -10 - AABBmin.x;
        dir = -dir;
    }
    else if (trans.x + AABBmax.x > 10) {
        trans.x = 10 - AABBmax.x;
        dir = -dir;
    }
    if (trans.z + AABBmin.z < -10) {
        trans.z = -10 - AABBmin.z;
        dir = -dir;
    }
    else if (trans.z + AABBmax.z > 10) {
        trans.z = 10 - AABBmax.z;
        dir = -dir;
    }
    doRotate(rotAxis, dt * rotSpeed);
    //doRotate(glm::vec3(0, 1, 0), 45);
    //TODO check for collisions
}

void GameObject::doTranslate(glm::vec3 trans) {
    this->trans += trans;
}

void GameObject::doRotate(glm::vec3 axis, float deg) {
    this->rotate = glm::rotate(this->rotate, deg, axis);
}

void GameObject::doScale(glm::vec3 scale) {
    this->scale.x *= scale.x;
    this->scale.y *= scale.y;
    this->scale.z *= scale.z;
}

bool GameObject::doesCollide(GameObject *other) {
    glm::vec3 ourAABBmin = this->AABBmin + this->trans;
    glm::vec3 ourAABBmax = this->AABBmax + this->trans;
    glm::vec3 theirAABBmin = other->AABBmin + other->trans;
    glm::vec3 theirAABBmax = other->AABBmax + other->trans;
    return (ourAABBmin.x < theirAABBmax.x &&
        ourAABBmin.y < theirAABBmax.y &&
        ourAABBmin.z < theirAABBmax.z &&
        theirAABBmin.x < ourAABBmax.x &&
        theirAABBmin.y < ourAABBmax.y &&
        theirAABBmin.z < ourAABBmax.z);
}

    /*glm::vec3 dir;
    float speed;
    glm::vec3 rotAxis;
    float rotSpeed
    glm::vec3 AABBmin;
    glm::vec3 AABBmax;
    GLuint IBO;
    int IBOlen;
    GLuint VBO;
    GLuint NBO;

    glm::vec3 scale;
    glm::mat4 rotate;
    glm::vec3 trans;
    */
