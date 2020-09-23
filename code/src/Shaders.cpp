#include "Shaders.h"

//--------------------------------------------------------------------------------------
// Crea los shaders de vértices y fragmentos a partir del código fuente correspondiente
//--------------------------------------------------------------------------------------
void Shaders::initShaders(const char *vShaderFile, const char *fShaderFile) { 

    unsigned int vShader = createShader(GL_VERTEX_SHADER  , vShaderFile);
    unsigned int fShader = createShader(GL_FRAGMENT_SHADER, fShaderFile);
    program = createProgram(vShader, fShader);
    
}

//--------------------------------------
// Crea un shader (vértices/fragmentos)
//--------------------------------------
unsigned int Shaders::createShader(unsigned long shader, const char* shaderFile) {
   
 // Se crea un objeto shader
    unsigned int shaderID = glCreateShader(shader); 
    
 // Se asigna su código fuente contenido en un fichero glsl
    std::string   code = "";
    std::ifstream file(shaderFile, std::ios::in);
    if(file.is_open()) {
        std::string line;
        while(getline(file, line)) code += line + "\n";
        file.close();
    }
    else {
        std::cout << "El fichero " << shaderFile << " no se puede abrir." << std::endl;
        return 0;
    }
    const char *shaderSrc = code.c_str();
    glShaderSource(shaderID, 1, &shaderSrc, NULL);
    
 // Se compila con control de errores
    int compiled;    
    glCompileShader(shaderID);
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compiled);
    if(compiled == GL_FALSE) {
        int logSize;
        glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &logSize);
        char *logMsg = new char[logSize];
        glGetShaderInfoLog(shaderID, logSize, NULL, logMsg);
        std::cout << "Error al compilar el Shader " << shaderFile << ": " << logMsg;
        delete[] logMsg;
        return 0;
    }
    
    return shaderID;
}

//-------------------------------------------------------------------
// Crea un programa(identificador) que usa los shaders especificados
//-------------------------------------------------------------------
unsigned int Shaders::createProgram(unsigned int vShader, unsigned int fShader) {
    
 // Se crea un objeto programa
    unsigned int program = glCreateProgram();
    
 // Se adjuntan los shaders y se eliminan los objetos correspondientes
    glAttachShader(program, vShader);
    glAttachShader(program, fShader);
    glDeleteShader(vShader);
    glDeleteShader(fShader);
    
 // Se enlaza el programa con control de errores
    int linked;    
    glLinkProgram(program);
    glGetShaderiv(program, GL_COMPILE_STATUS, &linked);
    if(linked == GL_FALSE) {
        int logSize;
        glGetShaderiv(program, GL_INFO_LOG_LENGTH, &logSize);
        char *logMsg = new char[logSize];
        glGetShaderInfoLog(program, logSize, NULL, logMsg);
        std::cout << "Error al enlazar el Shader: " << logMsg;
        delete[] logMsg;
        return 0;
    }
    
    return program;    
}

//-----------------------------------------------------
// Fija el valor de una variable uniforme de tipo vec3
//-----------------------------------------------------
void Shaders::setVec3(const std::string &name, glm::vec3 value) {
    
   glUniform3fv(glGetUniformLocation(program,name.c_str()), 1, glm::value_ptr(value));
    
}

//-----------------------------------------------------
// Fija el valor de una variable uniforme de tipo mat4
//-----------------------------------------------------
void Shaders::setMat4(const std::string &name, glm::mat4 value) {
    
   glUniformMatrix4fv(glGetUniformLocation(program,name.c_str()), 1, GL_FALSE, glm::value_ptr(value)); 
    
}

//------------------------------------------------------
// Fija el valor de una variable uniforme de tipo Light
//------------------------------------------------------
void Shaders::setLight(const std::string &name, Light value) {
    
    glUniform3fv(glGetUniformLocation(program,(name+".position"   ).c_str()), 1, glm::value_ptr(value.position ));
    glUniform3fv(glGetUniformLocation(program,(name+".direction"  ).c_str()), 1, glm::value_ptr(value.direction));
    glUniform3fv(glGetUniformLocation(program,(name+".ambient"    ).c_str()), 1, glm::value_ptr(value.ambient  ));
    glUniform3fv(glGetUniformLocation(program,(name+".diffuse"    ).c_str()), 1, glm::value_ptr(value.diffuse  ));
    glUniform3fv(glGetUniformLocation(program,(name+".specular"   ).c_str()), 1, glm::value_ptr(value.specular ));
    glUniform1f (glGetUniformLocation(program,(name+".innerCutOff").c_str()), glm::cos(glm::radians(value.innerCutOff)));
    glUniform1f (glGetUniformLocation(program,(name+".outerCutOff").c_str()), glm::cos(glm::radians(value.outerCutOff)));
    glUniform1f (glGetUniformLocation(program,(name+".c0"         ).c_str()), value.c0);
    glUniform1f (glGetUniformLocation(program,(name+".c1"         ).c_str()), value.c1);
    glUniform1f (glGetUniformLocation(program,(name+".c2"         ).c_str()), value.c2);
            
}

//---------------------------------------------------------
// Fija el valor de una variable uniforme de tipo Material
//---------------------------------------------------------
void Shaders::setMaterial(const std::string &name, Material value) {
    
    glUniform4fv(glGetUniformLocation(program,(name+".ambient"  ).c_str()), 1, glm::value_ptr(value.ambient ));
    glUniform4fv(glGetUniformLocation(program,(name+".diffuse"  ).c_str()), 1, glm::value_ptr(value.diffuse ));
    glUniform4fv(glGetUniformLocation(program,(name+".specular" ).c_str()), 1, glm::value_ptr(value.specular));
    glUniform4fv(glGetUniformLocation(program,(name+".emissive" ).c_str()), 1, glm::value_ptr(value.emissive));
    glUniform1f (glGetUniformLocation(program,(name+".shininess").c_str()), value.shininess);
            
}

//--------------------------------------------------------------------
// Fija el valor de una variable uniforme (sampler2D) de tipo Texture
//--------------------------------------------------------------------
void Shaders::setTextures(const std::string &name, Textures value) {
   
    glActiveTexture(GL_TEXTURE0 + value.diffuse);
    glBindTexture(GL_TEXTURE_2D,value.diffuse);
    glUniform1i(glGetUniformLocation(program,(name+".diffuse").c_str()), value.diffuse);
    
    glActiveTexture(GL_TEXTURE0 + value.specular);
    glBindTexture(GL_TEXTURE_2D,value.specular);
    glUniform1i(glGetUniformLocation(program,(name+".specular").c_str()), value.specular);
    
    glActiveTexture(GL_TEXTURE0 + value.emissive);
    glBindTexture(GL_TEXTURE_2D,value.emissive);
    glUniform1i(glGetUniformLocation(program,(name+".emissive").c_str()), value.emissive);
    
    if(value.normal!=0) { 
        glUniform1i(glGetUniformLocation(program,"unormals"),1);
        glActiveTexture(GL_TEXTURE0 + value.normal);
        glBindTexture(GL_TEXTURE_2D,value.normal);
        glUniform1i(glGetUniformLocation(program,(name+".normal"  ).c_str()), value.normal);
        
    }
    else {
        glUniform1i(glGetUniformLocation(program,"unormals"),0);
    }
    
    glUniform1f (glGetUniformLocation(program,(name+".shininess").c_str()), value.shininess);
            
}

//------------------------------------------------------
// Fija el valor de una variable uniforme de tipo float
//------------------------------------------------------
void Shaders::setFloat(const std::string &name, float value) {
    
    //glUniform1i(glGetUniformLocation(program,name.c_str()),value); esto esta mal: es para una variable tipo GLint
    glUniform1f(glGetUniformLocation(program,name.c_str()),value);
            
}

//------------------------------------------------------
// Fija el valor de una variable uniforme de tipo bool
//------------------------------------------------------
void Shaders::setBool(const std::string &name, int value) {
    
    //Either the i, ui or f variants may be used to provide values for uniform 
    //variables of type bool, bvec2, bvec3, bvec4, or arrays of these. The uniform 
    //variable will be set to false if the input value is 0 or 0.0f, and it will 
    //be set to true otherwise. -khronos.org-
    //glUniform1f(glGetUniformLocation(program,name.c_str()),value);
    glUniform1i(glGetUniformLocation(program,name.c_str()),value);
            
}

//-----------------------------------------
// Usa el shader para renderizar la escena
//-----------------------------------------
void Shaders::useShaders() {
    
    glUseProgram(program);
    
}

//-----------------------------------
// Destructor de la clasede la clase 
//-----------------------------------
Shaders::~Shaders() { 
    glDeleteProgram(program);
}
