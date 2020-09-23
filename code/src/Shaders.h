#ifndef SHADERS_H
#define SHADERS_H

#include <iostream>
#include <fstream>
#include <ogl/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Texture.h"

struct Light {
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float     innerCutOff;
    float     outerCutOff;
    float     c0;
    float     c1;
    float     c2;
};

struct Material {
    glm::vec4 ambient;
    glm::vec4 diffuse;
    glm::vec4 specular;
    glm::vec4 emissive;
    float     shininess;
};

struct Textures {
    unsigned int diffuse;
    unsigned int specular;
    unsigned int emissive;
    unsigned int normal;
    float        shininess;
};

class Shaders {
    
    public:
        
        void initShaders(const char *vShaderFile, const char *fShaderFile);
        void useShaders();
        
        void setVec3    (const std::string &name, glm::vec3 value);
        void setMat4    (const std::string &name, glm::mat4 value);
        void setLight   (const std::string &name, Light     value);
        void setMaterial(const std::string &name, Material  value);
        void setTextures(const std::string &name, Textures  value);
        void setFloat   (const std::string &name, float     value);
        void setBool    (const std::string &name, int       value);
        
        virtual ~Shaders();
                
    private:
                   
        unsigned int program;
                
        unsigned int createShader (unsigned long shader , const char *shaderFile);
        unsigned int createProgram(unsigned int  vShader, unsigned int fShader);

};

#endif /* SHADERS_H */
