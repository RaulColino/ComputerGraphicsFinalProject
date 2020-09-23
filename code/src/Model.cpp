#include "Model.h"


//-----------------------------------------------------------------------------------------------------
// Lee los atributos del modelo de un fichero de texto y los almacena en los vectores correspondientes
// despues crea varios VBO y un EBO para posteriormente almacenarlos en un VAO que representa el modelo y que 
// podra reconocer la tarjeta grafica. Con el VAO la grafica podra renderizar los vertices del modelo.
//-----------------------------------------------------------------------------------------------------
void Model::initModel(const char *modelFile) {

    // Importa el modelo mediante la librería Assimp
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(modelFile,
            aiProcess_Triangulate |
            aiProcess_JoinIdenticalVertices |
            aiProcess_PreTransformVertices |
            aiProcess_GenSmoothNormals |
            aiProcess_CalcTangentSpace |
            aiProcess_GenUVCoords);
    if (!scene) {
        std::cout << "El fichero " << modelFile << " no se puede abrir." << std::endl;
        std::cin.get();
        exit(1);
    }

    // Extrae los atributos (posiciones, normales, texturas, indices) del modelo importado
    aiNode *node = scene->mRootNode;
    aiMesh *mesh = scene->mMeshes[node->mMeshes[0]];
    for (int i = 0; i < mesh->mNumVertices; i++) {
        positions.push_back(glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z));
        normals.push_back(glm::normalize(glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z)));
        if (mesh->mTextureCoords[0]) textureCoords.push_back(glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y));
        else textureCoords.push_back(glm::vec2(0.0f, 0.0f));
    }
    for (int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (int j = 0; j < face.mNumIndices; j++) indices.push_back((GLuint)face.mIndices[j]);
    }
    
    // Crea un VAO con los atributos del modelo
    createVAO();
  
}


//-------------------------------------------------------------
// Creacion del modelo del terreno definido en la clase Terrain
//-------------------------------------------------------------
void Model::initTerrainModel(Terrain &t) {

    int terrainX  = t.terrainX;
    int terrainZ  = t.terrainZ;
    
    int nVertices = terrainX*terrainZ;
    int nIndices  = (terrainX - 1)*(terrainZ - 1)*6;

    //Llenamos los arrays de vectores con las pòsiciones, normales y coord de textura del terreno
    for (int i = 0; i < nVertices; i++) {
        positions.push_back(glm::vec3(t.vertices[i].x, t.vertices[i].y, t.vertices[i].z));
        normals.push_back(glm::normalize(glm::vec3(t.terrainNormals[i].x, t.terrainNormals[i].y, t.terrainNormals[i].z)));
        textureCoords.push_back(t.terrainTextureCoords[i]);
    }
    //llenamos el array con los indices de los vertices que forman cada triangulo
    for (int j = 0; j < nIndices; j++) {
        indices.push_back((GLuint)t.terrainIndices[j]);  
    } 
    
    // Crea un VAO con los atributos del modelo
    createVAO();

}


//--------------------------------
// Actualiza el modelo del terreno
//--------------------------------
void Model::updateTerrainModel(Terrain &t) {

    int terrainX  = t.terrainX;
    int terrainZ  = t.terrainZ;
    
    int nVertices = terrainX*terrainZ;
    
    //vaciamos los buffers
    positions.clear();
    normals.clear();
    
    //Llenamos los arrays de vectores con las pòsiciones, normales y coord de textura del terreno
    for (int i = 0; i < nVertices; i++) {
        positions.push_back(glm::vec3(t.vertices[i].x, t.vertices[i].y, t.vertices[i].z));
        normals.push_back(glm::normalize(glm::vec3(t.terrainNormals[i].x, t.terrainNormals[i].y, t.terrainNormals[i].z)));
    }
    
    createVAO();
}


//---------------------------------------------------
// Creacion un VAO y sus correspondiantes VBOs y EBOs
//---------------------------------------------------
void Model::createVAO(){
    
    //positions, normals, textureCoords, indices
    glGenVertexArrays(1, &vao);
    //se crea 1 identificador para el buffer de posiciones, normales, coord. de textura e indices.
    //(numero de identificadores de vbo a asignar en la variable vboPositions,direccion de la variable vboPositions) 
    glGenBuffers(1, &vboPositions);
    glGenBuffers(1, &vboNormals);
    glGenBuffers(1, &vboTextureCoords);
    glGenBuffers(1, &eboIndices);
    //se activa el VAO para cargar en el los buffers de posiciones, normales, coords.textura e indices
    glBindVertexArray(vao);
        // Posiciones
        //indicamos que la variable identificador vboPositions es el id de un GL_ARRAY_BUFFER (vertex buffer o vbo)
        glBindBuffer(GL_ARRAY_BUFFER, vboPositions);
        //se cargan los datos: (target, size, data: puntero al primer elemento del vector, usage)
        glBufferData(GL_ARRAY_BUFFER, sizeof (glm::vec3) * positions.size(), &(positions.front()), GL_STATIC_DRAW);
        //se asigna al atributo 0 definido en el shader de vertices: (en vshader.glsl -> layout (location = 0) in vec3 inPosition;)
        //(atributo, numero de componentes, tipo , normaliza, stride, offset);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        //Se activa el atributo 0 del shader de vertices 
        glEnableVertexAttribArray(0);
        // Normales   
        glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
        glBufferData(GL_ARRAY_BUFFER, sizeof (glm::vec3) * normals.size(), &(normals.front()), GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(1);
        // Texturas
        glBindBuffer(GL_ARRAY_BUFFER, vboTextureCoords);
        glBufferData(GL_ARRAY_BUFFER, sizeof (glm::vec2) * textureCoords.size(), &(textureCoords.front()), GL_STATIC_DRAW);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(2);
        // Índices
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboIndices);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof (GLuint) * indices.size(), &(indices.front()), GL_STATIC_DRAW);
    //se desactiva el VAO 
    glBindVertexArray(0);
    
}

//--------------------------------
// Renderiza el VAO con el modelo
//--------------------------------
void Model::renderModel(unsigned long mode) {

    glPolygonMode(GL_FRONT_AND_BACK, mode);
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void *) 0);
    glBindVertexArray(0);

}

//-----------------------------------
// Destructor de la clase
//-----------------------------------
Model::~Model() {

    glDeleteBuffers(1, &vao);
    glDeleteBuffers(1, &vboPositions);
    glDeleteBuffers(1, &vboNormals);
    glDeleteBuffers(1, &vboTextureCoords);
    glDeleteBuffers(1, &eboIndices);

}
