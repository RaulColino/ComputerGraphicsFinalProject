#include "Terrain.h"
#include "Shaders.h"


//-----------------------------------------
// Crea la malla del terreno con su relieve 
//-----------------------------------------
void Terrain::generateTerrain() {

    //Si las dimensiones no son correctas termina el programa con un error
    if (terrainX <= 1 || terrainZ <= 1) {
        std::cout << "generateTerrainMesh:Error: dimensiones del terreno no validas: " << terrainX << "x" << terrainZ << "\n";
        exit(1);
    }
    
    //Numero de vertices del modelo del terreno
    int nVertices = terrainX*terrainZ;
    
    //Generacion de alturas del terreno
        //Creamos los arrays que pasaremos al metodo 'generateTerrainHeight'
        float *noise_seed = nullptr;
        float *noise_values = nullptr;
        noise_seed = new float[nVertices];
        noise_values = new float[nVertices];
        //Generamos el array de valores aleatorios entre 1 y 0 que almacenara 'noise_seed'
        for (int k = 0; k < terrainZ; k++) {
            for (int i = 0; i < terrainX; i++) {
                noise_seed[k * terrainZ + i] = (float) rand() / (float) RAND_MAX;
            }
        }
        //Variables moduladoras del relieve
        float scalingBias = 2.2f; //factor de 'suavizado' que regula la altura de los vertices contiguos. Un menor valor crea picos mas abruptos en el terreno.
        int nOctaves = calculateMaxOctaves(); //128x128 -> 7 octaves max. 256x256 -> 8 octaves max. Define la frecuencia de ruido en el terreno(numero de iteraciones de generacion de ruido). Un mayor valor crea mas rugosidad/variabilidad en el terreno
        //Llamamos a la funcion que asignara valores al array de floats 'noise_values'
        generateTerrainHeight(terrainX, terrainZ, noise_seed, nOctaves, scalingBias, noise_values);

    //Creacion de vertices del terreno y coordenadas de textura
        float tiles = 8; //tiles x tiles imagenes sobre el terreno
        //float scaleFactor = 1/texScale;
        //Valores de ajuste para centrar el terreno al eje de cordenadas
        float offsetX = (float) terrainX * 0.5 - 0.5;
        float offsetZ = (float) terrainZ * 0.5 - 0.5;
        int pos = 0;
        for (int k = 0; k < terrainZ; k++) {
            for (int i = 0; i < terrainX; i++) {

                //Posiciones
                vertices[pos].x = i - offsetX;
                // 600 es la altura y anchura de la pantalla
                vertices[pos].y = -noise_values[k * terrainZ + i]*(180.0f) + 30.0f; //rango de altura del vertice (-150, 30)
                vertices[pos].z = k - offsetZ;
                //std::cout << "(" << terrainVertices[pos].x << ", " << terrainVertices[pos].y << ", " << terrainVertices[pos].z << ")";
                pos++;

                //Coordenadas de textura
                //float s = i*texScale; 
                //float t = k*texScale;
                float s = (i*tiles)/(terrainX-1); 
                float t = (k*tiles)/(terrainX-1);
                //if(k<3)
                //std::cout << "texCoord: (" << s << ", " << t << ")" << std::endl;
                terrainTextureCoords[k*terrainX + i] = glm::vec2(s, t);    
            }
        }

    //Creamos los indices y normales para la generacion de la malla del terreno
        //Inicializamos a cero las normales del terreno
        for (int i = 0; i < nVertices; i++) {
            terrainNormals[i].x = 0;
            terrainNormals[i].y = 0;
            terrainNormals[i].z = 0;
        }
        //Rellenamos el array de indices y de normales 
        for (int k = 0; k < terrainZ - 1; k++) {
            for (int i = 0; i < terrainX - 1; i++) {

                int v0 = k * terrainX + i;
                int v1 = k * terrainX + i + 1;
                int v2 = (k + 1) * terrainX + i;
                int v3 = (k + 1) * terrainX + i + 1;

                int p = (k * (terrainX - 1) + i)*6;

                //Indices
                //Triangulo de arriba
                //v0 -v1
                //  \ |
                //   v3
                terrainIndices[p]     = v0;
                terrainIndices[p + 1] = v3;
                terrainIndices[p + 2] = v1;
                //Triangulo de abajo
                //v0 
                // | \
                //v2- v3
                terrainIndices[p + 3] = v0;
                terrainIndices[p + 4] = v2;
                terrainIndices[p + 5] = v3;

                //Normales
                //Triangulo de arriba
                glm::vec3 n1 = glm::cross((vertices[v0] - vertices[v1]), (vertices[v3] - vertices[v1])); //calculo de prod. vect. en sentido antihorario siempre
                terrainNormals[v0]+= n1; //Se asigna la normal media del vertice en funcion de las normales de las caras adyacentes a este
                terrainNormals[v3]+= n1; //En esta parte no normalizamos los vectores. Al hacerlo nos aseguramos de que consideramos el area (tamaño) de cada 
                terrainNormals[v1]+= n1; //triangulo adyacente al vertice, ya que las aristas pueden tener diferentes longitudes. 
                //Triangulo de abajo
                glm::vec3 n2 = glm::cross((vertices[v3] - vertices[v2]), (vertices[v0] - vertices[v2])); 
                terrainNormals[v0]+= n2;
                terrainNormals[v2]+= n2;
                terrainNormals[v3]+= n2;

            }
        }

}

//------------------------------------
// Actualiza las normales del terreno. 
//------------------------------------
void Terrain::recomputeNormals(){
    
    //Reseteamos el array de normales
    for (int i = 0; i < terrainX*terrainZ; i++)
        terrainNormals[i] = glm::vec3(0.0,0.0,0.0);
    //Recalculamos las normales  
    for (int k = 0; k < terrainZ - 1; k++) {
        for (int i = 0; i < terrainX - 1; i++) {
            
            int v0 = k * terrainX + i;
            int v1 = k * terrainX + i + 1;
            int v2 = (k + 1) * terrainX + i;
            int v3 = (k + 1) * terrainX + i + 1;
            
            //Triangulo de arriba
            glm::vec3 n1 = glm::cross((vertices[v0] - vertices[v1]), (vertices[v3] - vertices[v1])); //calculo de prod. vect. en sentido antihorario siempre
            terrainNormals[v0] += n1; //Se asigna la normal media del vertice en funcion de las normales de las caras adyacentes a este
            terrainNormals[v3] += n1; //En esta parte no normalizamos los vectores. Al hacerlo nos aseguramos de que consideramos el area (tamaño) de cada 
            terrainNormals[v1] += n1; //triangulo adyacente al vertice, ya que las aristas pueden tener diferentes longitudes. 
            //Triangulo de abajo
            glm::vec3 n2 = glm::cross((vertices[v3] - vertices[v2]), (vertices[v0] - vertices[v2]));
            terrainNormals[v0] += n2;
            terrainNormals[v2] += n2;
            terrainNormals[v3] += n2;
            
        }
    }   

}

//---------------------------------------------------------------------------------------
// Algoritmo tipo "Perlin noise" que asigna valores de altura a los vertices del terreno.
// Genera la base del relieve del terreno formando en el centro una zona cuadrada y plana.
//---------------------------------------------------------------------------------------
void Terrain::generateTerrainHeight(int terrainX, int terrainZ, float *noise_seed, int nOctaves, float scalingBias, float *noise_output) {

    //Para cada uno de los vertices del terreno se crea el valor de la altura
    for (int z = 0; z < terrainZ; z++) {
        for (int x = 0; x < terrainX; x++) {

            float noise = 0.0f;            //valor de ruido a calcular
            float scale = 1.0f;            //factor de escala a acumular en scaleAccumulated
            float scaleAccumulated = 0.0f; //valor necesario para mantener el rango entre 0 y 1

            //Por cada iteracion se obtiene un ruido mas detallado
            for (int octave_i = 0; octave_i < nOctaves; octave_i++) {

                //se toman 2 puntos a partir de los cuales se coge un valor de la semilla (array de valores aleatorios pasado por parametro)
                int pitch_x = terrainX >> octave_i; //equivalente a terrainX / (2*octave_i) 
                int pitch_z = terrainZ >> octave_i;

                int samplePoint1_x = (x / pitch_x) * pitch_x; //ojo: operamos con enteros, no con decimales.
                int samplePoint1_z = (z / pitch_z) * pitch_z;

                int samplePoint2_x = (samplePoint1_x + pitch_x) % terrainX;
                int samplePoint2_z = (samplePoint1_z + pitch_z) % terrainZ;

                //Interpolacion linear entre los puntos tomados (esto hace que haya cierta 'coherencia' entre vertices adyacentes del terreno)
                float blendX = (float) (x - samplePoint1_x) / (float) pitch_x;
                float blendZ = (float) (z - samplePoint1_z) / (float) pitch_z;

                float samplePointT = (1.0f - blendX) * noise_seed[samplePoint1_z * terrainX + samplePoint1_x] + blendX * noise_seed[samplePoint1_z * terrainX + samplePoint2_x];
                float samplePointB = (1.0f - blendX) * noise_seed[samplePoint2_z * terrainX + samplePoint1_x] + blendX * noise_seed[samplePoint2_z * terrainX + samplePoint2_x];

                //se acumula el valor de ruido y se acualiza el factor de escalado
                noise += (blendZ * (samplePointB - samplePointT) + samplePointT) * scale;
                scaleAccumulated += scale;
                scale = scale / scalingBias;

            }

            //asigna un valor entre 1 y 0
            noise_output[z * terrainX + x] = noise / scaleAccumulated;
     
        }
    }
    
}

//----------------------------------------------------------------------------
// Algoritmo que simula la caida, el trayecto y la consecuente erosion de una 
// gota de agua de agua sobre un punto aleatorio del terreno. 
//----------------------------------------------------------------------------
void Terrain::erode(float minVol, float dt, float density, float friction, float depositionRate, float evapRate){
    
    //Creamos una particula de agua en un punto aleatorio del terreno
    glm::vec2 newpos = glm::vec2(rand()%(terrainX-2)+1, rand()%(terrainZ-2)+1);
    Particle drop(newpos);
    
    //Mientras la gota no se haya evaporado...
    while(drop.volume > minVol){
        
      glm::ivec2 ipos = drop.pos;                         //Posicion inicial de la gota
      glm::vec3  n    = surfaceNormal(ipos.x, ipos.y);  //Vector normal en esa posicion
      
      //A traves del vector normal se calcula la velocidad, direccion y tiempo en alcanzar un nuevo vertice 
      drop.speed += dt*glm::vec2(n.x, n.z)/(drop.volume*density);// v = dt*F/m
      float dtUnit = 1 / length(normalize(drop.speed));
      drop.pos   += dtUnit*normalize(drop.speed);
      drop.speed *= (1.0-dtUnit*friction);       //Factor de friccion
      

      //Comprobamos si la particula sigue dentro de sus limites
      if(!glm::all(glm::greaterThanEqual(drop.pos, glm::vec2(1.0,1.0))) ||
         !glm::all(glm::lessThan(drop.pos, glm::vec2((float)terrainX-1, (float)terrainZ-1)))) break;
      
      //Compute sediment capacity difference
      //Calculamos el deficit/exceso de sedimento de la gota en el nuevo vertice
      float maxsediment = drop.volume*glm::length(drop.speed)*(vertices[ipos.x+(ipos.y*terrainX)].y - vertices[(int)drop.pos.x+((int)drop.pos.y*terrainX)].y);
      if(maxsediment < -100.0) maxsediment = -100.0;
      float sdiff = maxsediment - drop.sediment;
      
      //Asignamos el nuevo valor calculado de sedimento
      drop.sediment += dtUnit*depositionRate*sdiff;
      
      //Aplicamos la erosion/deposito de sedimento en el nuevo vertice del terreno
      float maxErosion = (vertices[ipos.x+(ipos.y*terrainX)].y - vertices[(int)drop.pos.x+((int)drop.pos.y*terrainX)].y)*0.5; 
      float  heigthDiff = std::min((dtUnit*drop.volume*depositionRate*sdiff),maxErosion); //corrige la formacion de picos en el terreno
      vertices[ipos.x+(ipos.y*terrainX)].y -= heigthDiff;
      
      //Evaporamos una parte de la gota
      drop.volume *= (1.0 - dtUnit*evapRate);
      
      //Cuando la gota apenas se mueve no esperamos a que se evapore para terminar la simulacion
      if(length(drop.speed) <= 0.01) break;
      
    }

}

//-----------------------------------------------------------------------
//Calcula el vector normal a la superficie de un vertice i,k del terreno
//-----------------------------------------------------------------------
glm::vec3 Terrain::surfaceNormal(int i, int k){
    
  //Two large triangels adjacent to the plane (+Y -> +X) (-Y -> -X)
    int ic  = k*terrainX + i;  //indice del vertice 
    int ilu = ic -terrainX -1; //vertice sup izq 
    int icu = ic -terrainX;    //vertice sup  
    int icr = ic -terrainX +1; //vertice sup der  
    int il  = ic-1;            //vertice izq  
    int ir  = ic+1;            //vertice der 
    int ild = ic+terrainX-1;   //vertice inf izq 
    int icd = ic+terrainX;     //vertice inf
    int ird = ic+terrainX+1;   //vertice inf der
    
    glm::vec3 vlu =  glm::vec3(-1.0f, vertices[ilu].y-vertices[ic].y,-1.0); //vector sup izq
    glm::vec3 vcu =  glm::vec3( 0.0f, vertices[icu].y-vertices[ic].y,-1.0); //vector sup
    glm::vec3 vr  =  glm::vec3( 1.0f, vertices[ir ].y-vertices[ic].y, 0.0); //vector sup der
    glm::vec3 vl  =  glm::vec3(-1.0f, vertices[il ].y-vertices[ic].y, 0.0);
    glm::vec3 vcd =  glm::vec3( 0.0f, vertices[icd].y-vertices[ic].y, 1.0);
    glm::vec3 vrd =  glm::vec3( 1.0f, vertices[ird].y-vertices[ic].y, 1.0);
            
    glm::vec3 n = glm::cross(vcu, vlu);
    n += glm::cross(vlu, vl);
    n += glm::cross(vl, vcd);
    n += glm::cross(vcd,vrd);
    n += glm::cross(vrd, vr);
    n += glm::cross(vr, vcu);

    return glm::normalize(n);
  
}

//----------------------------------------------------------------------
// Calcula el maximo de iteraciones del algoritmo de generacion de ruido
// para agregar el maximo numero de niveles de detalle al terreno
//----------------------------------------------------------------------
int Terrain::calculateMaxOctaves(){

    int factor = 1;
    int result = 2;
    while (result < terrainX) {
        result *= 2;
        factor++;
    }
    return factor;
    
}

//-----------------------------------
// Contructor de la clase
//-----------------------------------
Terrain::Terrain(int terrainX, int terrainZ) {
    
    this->terrainX = terrainX;
    this->terrainZ = terrainZ;

    int nVertices = terrainX*terrainZ;
    int nIndices = (terrainX - 1)*(terrainZ - 1)*6;

    vertices             = new glm::vec3[nVertices];
    terrainNormals       = new glm::vec3[nVertices];
    terrainIndices       = new GLuint   [nIndices];
    terrainTextureCoords = new glm::vec2[nVertices];

}

//-----------------------------------
// Destructor de la clase
//-----------------------------------
Terrain::~Terrain() {

    delete []vertices;
    delete []terrainNormals;
    delete []terrainIndices;
    delete []terrainTextureCoords;

}





