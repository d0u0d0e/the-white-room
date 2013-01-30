using glm::ivec2;
using glm::vec2;
using glm::vec3;
using glm::vec4;

using glm::mat2;
using glm::mat3;
using glm::mat4;

#include <GL/gl.h>

typedef struct GameConstants {
    GLuint shader;
    float aspectRatio;
    glm::vec3 lightPos;
    glm::vec3 lightColor;
    GLuint h_aPosition;
    GLuint h_aNormal;
    GLuint h_uColor;
    GLuint h_uProjMatrix;
    GLuint h_uViewMatrix;
    GLuint h_uModelMatrix;
    GLuint h_uNormalMatrix;
    GLuint h_uLightPos;
    GLuint h_uLightColor;
    GLuint h_uShininess;
    GLuint h_uSpecStrength;
    GLuint h_uCamTrans;
} GameConstants;

class GameObject {
    public:
    GameObject(GLuint VBO, GLuint IBO, int IBOlen);
    GameObject();
    ~GameObject();
    void draw(glm::vec3 cameraPos, glm::vec3 lookAt,
        glm::vec3 lightPos, glm::vec3 lightColor, GameConstants gc);
    void update(float dt);
    bool doesCollide(GameObject *other);
    void doTranslate(glm::vec3 trans);
    void doRotate(glm::vec3 axis, float deg);
    void doScale(glm::vec3 scale);

    glm::vec3 dir;
    float speed;
    glm::vec3 rotAxis;
    float rotSpeed;

    // TODO: matrix transforms (orientation!)
    glm::vec3 AABBmin;
    glm::vec3 AABBmax;
    GLuint IBO;
    int IBOlen;
    GLuint VBO;
    GLuint NBO;

    glm::vec3 color;
    float shininess;
    float specStrength;
    int tag; //Misc information goes here

    GameObject *next;

    //glm::mat4 scale;
    glm::vec3 scale;
    glm::mat4 rotate;
    glm::vec3 trans;
    //glm::mat4 trans;
};
