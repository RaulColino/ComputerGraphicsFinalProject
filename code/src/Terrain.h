#ifndef TERRAIN_H
#define TERRAIN_H

#include <iostream>
#include <vector>
#include <ogl/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

struct Particle{
  //Construct Particle at Position
  Particle(glm::vec2 _pos){ pos = _pos; }
  glm::vec2 pos;
  glm::vec2 speed = glm::vec2(0.0);
  float volume = 1.0;   //Varia en el tiempo
  float sediment = 0.0; //Fraccion de volumen que se convierte en sedimento
};

class Terrain {

    public:
        
        //Atributos
        int        terrainX;
        int        terrainZ;
        glm::vec3* vertices;
        glm::vec3* terrainNormals;
        GLuint   * terrainIndices; //para un terreno de 128x128 necesitamos 96774 indices. GLushort tiene 16 bits de profundidad, el rango de valores que puede representar es 0-65536. Por lo tanto necesitamos GLuint.
        glm::vec2* terrainTextureCoords;
        //Metodos
        void generateTerrain();
        void recomputeNormals();
        void erode(float minVol, float dt, float density, float friction, float depositionRate, float evapRate);
        //Contructor
        Terrain(int terrainX, int terrainZ);
        //destructor
        ~Terrain(); //hay que eliminar el espacio revervado para los atributos de la clase en el heap

    private:
        
        void      generateTerrainHeight(int terrainX, int terrainZ, float *noise_seed, int nOctaves, float scalingBias, float *noise_output);
        glm::vec3 surfaceNormal(int i, int k);
        int       calculateMaxOctaves();

};


#endif /* TERRAIN_H */

