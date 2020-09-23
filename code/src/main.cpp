// Practica 4
#include <iostream>
#include <chrono>
#include <thread>
#include <ogl/glew.h>
#include <ogl/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shaders.h"
#include "Model.h"
#include "Terrain.h"

#include <sstream>
std::string toString(const int &i) {
    std::stringstream ss;
    ss << i;
    return ss.str();
}

#define I glm::mat4(1.0)


/// Callbacks ------------------------------------------------------------------
void funInit();
void funReshape(int w, int h);
void funDisplay();
void funMouse(int button, int state, int x, int y);
void funMotion(int x, int y);
void funKeyboard(unsigned char key, int x, int y);
void funTimer(int ignore);
void funSpecial(int key, int x, int y);


/// Funciones ------------------------------------------------------------------
void setLights(glm::mat4 P, glm::mat4 V);
void drawObjectMat(Model model, Material material, glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawObjectTex(Model model, Textures textures, glm::mat4 P, glm::mat4 V, glm::mat4 M); 
void drawTerrain(Model model, Textures texFlat,Textures texSteep, glm::mat4 P, glm::mat4 V, glm::mat4 M);
void applyTerrainErosion(int nIterations);
void drawAspa(glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawAspa2(glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawHelice(glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawHelice2(glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawCuerpo(glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawCabina(glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawCola(glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawBase(glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawHelicoptero(glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawPuertasA(glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawPuertasB(glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawHeliceHelicoptero2(glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawCristal(glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawHelicoptero2(glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawPlataforma(glm::mat4 P, glm::mat4 V, glm::mat4 M);


/// Variables globales ---------------------------------------------------------
// Shaders
   Shaders shaders;

// Modelos
   Model terrainModel;
   Model esfera;
   Model cono;
   Model cilindro;
   Model helicopteroCuerpo;
   Model helicopteroHelice;
   Model puertasA;
   Model puertasB;
   Model puertaC;
   Model plataforma;
   Model cristal;
   
// Texturas
   Texture textureEmissive;
   Texture textureNoEmissive;
   Texture textureGrass;
   Texture textureGrassSpecular;
   Texture textureGrassNormal; 
   Texture textureRock;
   Texture textureRockNormal;
   //Helicotero 
   Texture textureHelicopter;
   Texture textureGlass;
   //Helipuerto
   Texture textureHelipuerto;
   //Puertas
   Texture textureDoors;


// Viewport
   int w = 600;
   int h = 600;

// Estado del botton izquierdo del raton
   bool leftPressed = false;

// Camara 
  // Angulo de enfoque 
     float fovy = 30.0;
  // Angulo de desplazamiento
     float alphaX = 0;
     float alphaY = 0;
  // Movimiento en el plano XZ
     float Z = -36.0;
     float X = -36.0;
  // Movimiento en el eje Y
     float Y = 16.0;

// Temporizador
     GLint speed = 20;      
// Animaciones del helicoptero 1
  // Rotacion de las helices 1
     float rotHelices1 = 90.f;
  // Rotacion de las helices 2
     float rotHelices2 = 120.f;
  // Detener la rotacion de las helices
     bool rotarHelices = true;  
  // Rotacion del helicoptero
     float rotHelicoptero = 0.f;
  // Desplazamiento del helicoptero en X 
     float desH1X = 5.0;
  // Desplazamiento del helicoptero en Y 
     float desH1Y = 15.5;
  // Desplazamiento del helicoptero en Z 
     float desH1Z = 0.0;
     
// Animaciones del helicoptero 2
  // Rotacion de las helices
     float rotHelices = 90.f;
  // Detener la rotacion de las helices
     bool rotarHelices2 = true; 
  // Rotacion del helicoptero
     float rotHelicoptero2 = 0.f;
  // Desplazamiento del helicoptero en X 
     float desH2X = -20.0;
  // Desplazamiento del helicoptero en Y 
     float desH2Y = 4.0;
  // Desplazamiento del helicoptero en Z 
     float desH2Z = 0.0;
  // Rotacion de las puertas A
     float rotPuertasA = 0.f;
  // Rotacion de las puertas B
     float rotPuertasB = 0.f;
     
     
 //Animaciones de las luces
     //Luz direccional
     float intensidad = 0.7;
     //Rotacion de la luz focal
     float rotFocal = 3.0;
     //Apagar/Encender luces posicionales
     float turnOff = false;
     
// Terreno
  // Dimensiones (numero de vertices del terreno)
    #define   TSIZE_X 256 
    #define   TSIZE_Z 256
  //Objeto 
    Terrain t(TSIZE_X, TSIZE_Z);
  //Variables de desplazamiento
    float desX=0.0f, desZ= 0.0f, desY=0.0f, rotX=0.0f;
  //variable para texturizar el terreno en funcion de la pendiente
    float steepness = 0.9;

    
// Luces y materiales
   #define   N_DLIGHTS 1
   #define   N_PLIGHTS 4
   #define   N_FLIGHTS 1
   Light     lightGlobal;
   Light     lightDirectional[N_DLIGHTS];
   Light     lightPositional[N_PLIGHTS];
   Light     lightFocal[N_FLIGHTS];
   
   Material  matRuby;
   Material  matGold;
   Material  matBronze;
   Material  matPerl;
   
   Textures  texGrass;
   Textures  texRock;
   Textures  texHelicopter;
   Textures  texHelipuerto;
   Textures  texDoors;
   Textures  texGlass;
   
   //Color de fondo
   glm::vec3 skyColor = glm::vec3(0.5804, 0.7331, 0.9101);
   //Intensidad de la luz direccional
   glm::vec3 bouncedLightIntensity = skyColor;
   glm::vec3 directLightIntensity  = glm::vec3(1.00, 0.89, 0.83);


// Modo de renderizado    
   bool wireframeMode = false;
   


/// Main -----------------------------------------------------------------------

int main(int argc, char** argv) {

 // Inicializamos GLUT
    glutInit(&argc, argv);
    glutInitContextVersion(3, 3);
    glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(w, h);
    glutInitWindowPosition(50, 50);
    glutCreateWindow("Practica 4");

 // Inicializamos GLEW
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        std::cout << "Error: " << glewGetErrorString(err) << std::endl;
        return false;
    }
    std::cout << "Status: Using GLEW " << glewGetString(GLEW_VERSION) << std::endl;
    const GLubyte *oglVersion = glGetString(GL_VERSION);
    std::cout << "This system supports OpenGL Version: " << oglVersion << std::endl;

 // Inicializaciones específicas
    funInit();

 // Configuración CallBacks
    glutReshapeFunc(funReshape);
    glutDisplayFunc(funDisplay);
    glutMouseFunc(funMouse);
    glutMotionFunc(funMotion);
    glutKeyboardFunc(funKeyboard);
    glutTimerFunc(speed, funTimer, 0);
    glutSpecialFunc(funSpecial);

 // Bucle principal
    glutMainLoop();

    return 0;
}

/// Implementacion de callbacks ------------------------------------------------

void funInit() { //se ejecuta siempre al principio y solo una vez

 // Test de profundidad
    glEnable(GL_DEPTH_TEST);
    glPolygonOffset(1.0, 1.0);
    glEnable(GL_CULL_FACE);
// Transparencias
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    

 // Shaders
    shaders.initShaders("resources/shaders/vshader.glsl", "resources/shaders/fshader.glsl");

 // Terreno
    t.generateTerrain();         // se genera un terreno
    auto start = std::chrono::high_resolution_clock::now();
    applyTerrainErosion(8000);   // se le aplica la erosion          
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> duration = end - start;
    std::cout << "t erosion:" << duration.count() << "s " << std::endl;

 // Modelos
    terrainModel.initTerrainModel(t);      // se genera el modelo del terreno
    esfera.initModel("resources/models/sphere.obj"); 
    cono.initModel("resources/models/cone.obj");
    cilindro.initModel("resources/models/cylinder.obj");
    helicopteroCuerpo.initModel("resources/models/body.obj");
    helicopteroHelice.initModel("resources/models/helix.obj");
    puertasA.initModel("resources/models/doorA.obj");
    puertasB.initModel("resources/models/doorB.obj");
    puertaC.initModel("resources/models/compartmentDoor.obj");
    plataforma.initModel("resources/models/plane.obj");
    cristal.initModel("resources/models/glass.obj");
    

 // Texturas
    textureEmissive.initTexture("resources/textures/imgEmissive.png");
    textureNoEmissive.initTexture("resources/textures/imgNoEmissive.png");
    textureGrass.initTexture("resources/textures/imgGrass.png");
    textureGrassSpecular.initTexture("resources/textures/imgGrassSpecular.png");
    textureGrassNormal.initTexture("resources/textures/imgGrassNormal.png");
    textureRock.initTexture("resources/textures/imgRock.png");
    textureRockNormal.initTexture("resources/textures/imgRockNormal.png");
    textureHelipuerto.initTexture("resources/textures/helipuerto.png");
    textureHelicopter.initTexture("resources/textures/imgBodyDiffuse.png");
    textureDoors.initTexture("resources/textures/steel.jpg");
    textureGlass.initTexture("resources/textures/imgGlass.png");
   
 // Luz ambiental global
    lightGlobal.ambient    = glm::vec3(0.5, 0.5, 0.5); //componente blanca y fija que queda tras apagar todas las luces (dir., pos., foc.)
    
 // Luces direccionales
    lightDirectional[0].direction   = glm::vec3(-1.0,-0.5, 0.0);
    lightDirectional[0].ambient     = glm::vec3(bouncedLightIntensity);
    lightDirectional[0].diffuse     = glm::vec3(directLightIntensity);
    lightDirectional[0].specular    = glm::vec3(directLightIntensity);
    
//  Luces posicionales
    lightPositional[0].position    = glm::vec3(0.0, 1.0,-15.0);
    lightPositional[0].ambient     = glm::vec3(0.2, 0.2, 0.2);
    lightPositional[0].diffuse     = glm::vec3(0.9, 0.9, 0.9);
    lightPositional[0].specular    = glm::vec3(0.9, 0.9, 0.9);
    lightPositional[0].c0          = 1.00;
    lightPositional[0].c1          = 0.22;
    lightPositional[0].c2          = 0.20;
    
    lightPositional[1].position    = glm::vec3(0.0, 1.0, 15.0);
    lightPositional[1].ambient     = glm::vec3(0.2, 0.2, 0.2);
    lightPositional[1].diffuse     = glm::vec3(0.9, 0.9, 0.9);
    lightPositional[1].specular    = glm::vec3(0.9, 0.9, 0.9);
    lightPositional[1].c0          = 1.00;
    lightPositional[1].c1          = 0.22;
    lightPositional[1].c2          = 0.20;
    
    lightPositional[2].position    = glm::vec3(15.0, 1.0, 0.0);
    lightPositional[2].ambient     = glm::vec3(0.2, 0.2, 0.2);
    lightPositional[2].diffuse     = glm::vec3(0.9, 0.9, 0.9);
    lightPositional[2].specular    = glm::vec3(0.9, 0.9, 0.9);
    lightPositional[2].c0          = 1.00;
    lightPositional[2].c1          = 0.22;
    lightPositional[2].c2          = 0.20;
    
    lightPositional[3].position    = glm::vec3(-15.0, 1.0, 0.0);
    lightPositional[3].ambient     = glm::vec3(0.2, 0.2, 0.2);
    lightPositional[3].diffuse     = glm::vec3(0.9, 0.9, 0.9);
    lightPositional[3].specular    = glm::vec3(0.9, 0.9, 0.9);
    lightPositional[3].c0          = 1.00;
    lightPositional[3].c1          = 0.22;
    lightPositional[3].c2          = 0.20;
 
 // Luces focales
    lightFocal[0].position    = glm::vec3( 0.0,  1.0, 13.0);
    lightFocal[0].direction   = glm::vec3( 0.0, -1.0, -13.0);
    lightFocal[0].ambient     = glm::vec3( 0.2,  0.2,  0.2);
    lightFocal[0].diffuse     = glm::vec3( 0.9,  0.9,  0.9);
    lightFocal[0].specular    = glm::vec3( 0.9,  0.9,  0.9);
    lightFocal[0].innerCutOff = 10.0;
    lightFocal[0].outerCutOff = lightFocal[0].innerCutOff + 1.0;
    lightFocal[0].c0          = 1.000;
    lightFocal[0].c1          = 0.090;
    lightFocal[0].c2          = 0.032;
    
 // Materiales
    matRuby.ambient       = glm::vec4(0.174500, 0.011750, 0.011750, 1.00);
    matRuby.diffuse       = glm::vec4(0.614240, 0.041360, 0.041360, 1.00);
    matRuby.specular      = glm::vec4(0.727811, 0.626959, 0.626959, 1.00);
    matRuby.emissive      = glm::vec4(0.000000, 0.000000, 0.000000, 1.00);
    matRuby.shininess     = 76.8;
    
    matGold.ambient       = glm::vec4(0.247250, 0.199500, 0.074500, 1.00);
    matGold.diffuse       = glm::vec4(0.751640, 0.606480, 0.226480, 1.00);
    matGold.specular      = glm::vec4(0.628281, 0.555802, 0.366065, 1.00);
    matGold.emissive      = glm::vec4(0.000000, 0.000000, 0.000000, 1.00);
    matGold.shininess     = 51.2;
    
    matBronze.ambient    = glm::vec4(0.250000, 0.148000, 0.064750, 0.65);
    matBronze.diffuse    = glm::vec4(0.400000, 0.236800, 0.103600, 0.65);
    matBronze.specular   = glm::vec4(0.774597, 0.458561, 0.200621, 0.65);
    matBronze.emissive   = glm::vec4(0.000000, 0.000000, 0.000000, 0.65);
    matBronze.shininess  = 76.8;
   
    matPerl.ambient    = glm::vec4(0.250000, 0.207250, 0.207250, 0.922);
    matPerl.diffuse    = glm::vec4(1.000000, 0.829000, 0.829000, 0.922);
    matPerl.specular   = glm::vec4(0.296648, 0.296648, 0.296648, 0.922);
    matPerl.emissive   = glm::vec4(0.800000, 0.800000, 0.800000, 0.65);
    matPerl.shininess  = 11.264;
    
    texGrass.diffuse     = textureGrass.getTexture();
    texGrass.specular    = textureGrassSpecular.getTexture();
    texGrass.emissive    = textureNoEmissive.getTexture();
    texGrass.normal      = textureGrassNormal.getTexture();  
    texGrass.shininess   = 10.0;
    
    texRock.diffuse      = textureRock.getTexture();
    texRock.specular     = textureNoEmissive.getTexture();
    texRock.emissive     = textureNoEmissive.getTexture();
    texRock.normal       = textureRockNormal.getTexture();  
    texRock.shininess    = 0.1;
    
    texHelipuerto.diffuse      = textureHelipuerto.getTexture();
    texHelipuerto.specular     = textureHelipuerto.getTexture();
    texHelipuerto.emissive     = textureEmissive.getTexture();
    texHelipuerto.normal       = 0.0;  
    texHelipuerto.shininess    = 10.0;
    
    texHelicopter.diffuse      = textureHelicopter.getTexture();
    texHelicopter.specular     = textureHelicopter.getTexture();
    texHelicopter.emissive     = textureNoEmissive.getTexture();
    texHelicopter.normal       = 0.0;  
    texHelicopter.shininess    = 10.0;
    
    texGlass.diffuse      = textureGlass.getTexture();
    texGlass.specular     = textureGlass.getTexture();
    texGlass.emissive     = textureNoEmissive.getTexture();
    texGlass.normal       = 0.0;  
    texGlass.shininess    = 10.0;
    
    texDoors.diffuse      = textureDoors.getTexture();
    texDoors.specular     = textureDoors.getTexture();
    texDoors.emissive     = textureNoEmissive.getTexture();
    texDoors.normal       = 0.0;  
    texDoors.shininess    = 10.0;
    
    
}


void funReshape(int wnew, int hnew) {

 // Configuración del Viewport
    glViewport(0, 0, wnew, hnew);

 // Captura de w y h
    w = wnew;
    h = hnew;

}


void funDisplay() {

 // Borramos el buffer de color
    glClearColor(skyColor.r, skyColor.g, skyColor.b, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

 // Indicamos los shaders a utilizar
    shaders.useShaders();

 // Matriz de Proyección P (Perspectiva)
    float nplane = 0.1;
    float fplane = 400.0;
    float aspect = (float) w / (float) h;
    glm::mat4 P = glm::perspective(glm::radians(fovy), aspect, nplane, fplane);
   
 // Matriz de Vista V (Cámara)
 // Calculo de la posicion de la camara
    float x2 = 100.0f * glm::cos(alphaY) * glm::sin(alphaX);
    float y2 = 100.0f * glm::sin(alphaY);
    float z2 = 100.0f * glm::cos(alphaY) * glm::cos(alphaX);
    glm::vec3 pos   (X, Y, Z);   
    glm::vec3 lookat(x2, y2, z2);
    glm::vec3 up    (0.0, 1.0, 0.0);
    glm::mat4 V = glm::lookAt(pos, lookat, up);
    //pasamos al shader de vertices la posicion de la camara
    shaders.setVec3 ("ucpos",pos);
    
 // Colocamos las luces
    setLights(P,V);   

 // Dibujamos la escena 
  
    //Terreno
     // Matriz M para desplazar el terreno
        float scale =  1.0;
        glm::mat4 S = glm::scale    (I, glm::vec3(scale,scale,scale));
        glm::mat4 R = glm::rotate   (I, glm::radians(rotX), glm::vec3(1.0, 0.0, 0.0));
        glm::mat4 T = glm::translate(I, glm::vec3(desX, desY, desZ));
        glm::mat4 M = T*R*S;
     // Dibujamos el terreno
        drawTerrain(terrainModel, texGrass, texRock, P, V, M);
        
        
    //Dibujamos los helicopteros
        //Animaciones del helicoptero 1
        glm::mat4 S2 = glm::scale    (I, glm::vec3(0.8,0.8,0.8));
        glm::mat4 T2 = glm::translate(I, glm::vec3(desH1X, desH1Y, desH1Z));
        glm::mat4 R2 = glm::rotate(I, glm::radians(rotHelicoptero), glm::vec3(0.0, 1.0, 0.0));
        
        drawHelicoptero(P,V,T2*R2*S2);
        
        //Animaciones del helicotero 2
        glm::mat4 T3 = glm::translate(I, glm::vec3(desH2X, desH2Y, desH2Z));
        glm::mat4 R3 = glm::rotate(I, glm::radians(rotHelicoptero2), glm::vec3(0.0, 1.0, 0.0));
        
        glDisable(GL_CULL_FACE);
        drawHelicoptero2(P,V,T3*R3);
        glEnable(GL_CULL_FACE);
        
    //Dibujamos la plataforma
        glDisable(GL_BLEND);
        //Dibujamos la cara de arriba de la plataforma
        drawPlataforma(P, V, I);
        //Dibujamos la cara de abajo de la plataforma
        glm::mat4 Rp = glm::rotate(I, glm::radians(180.0f), glm::vec3(1.0, 0.0, 0.0));
        drawPlataforma(P, V, Rp);
        glEnable(GL_BLEND);
        
        
    //Dibujamos los elementos transparentes
        
        glDepthMask(GL_FALSE);
        glDisable(GL_CULL_FACE);
        
        drawCabina(P,V,T2*R2*S2);
        drawCristal(P,V,T3*R3);
        
        glEnable(GL_CULL_FACE);
        glDepthMask(GL_TRUE);


 // Intercambiamos los buffers
    glutSwapBuffers();

}


void funMouse(int button, int state, int x, int y) {

    if (state == GLUT_DOWN) { //boton presionado

        switch (button) {
            case GLUT_LEFT_BUTTON:
                leftPressed = true;
                break;
            case 3: //mouse wheel scroll up      
                if (fovy > 10)
                    fovy--;
                break;
            case 4: //mouse wheel scroll down
                if (fovy < 120)
                    fovy++;
                break;
            default:
                break;
        }

    } else { //boton soltado

        if (button = GLUT_LEFT_BUTTON) {
            leftPressed = false;
            std::cout << "left release at: (" << x << ", " << y << ")\n";
        }
    }
    glutPostRedisplay();

}


void funMotion(int x, int y) {

    if (leftPressed) {

        //coordenadas desde el centro de la pantalla
        int posX = x - (w / 2);
        int posY = (h / 2) - y;

        //angulo de desplazamiento de la camara
        float aX = posX * 0.3;
        float aY = posY * 0.3;

        //if (aX > -179 && aX < 179)
            alphaX = glm::radians(aX);

        if (aY > -89 && aY < 89)
            alphaY = glm::radians(aY);

        
        glutPostRedisplay();
    }

}


void funKeyboard(unsigned char key, int x, int y) {

    switch (key) {
        //Terreno
        case 't':
            steepness = glm::max(steepness-0.05, 0.0);
            std::cout << "steepness: "<< steepness  << std::endl;
            break;           
        case 'T':
            steepness = glm::min(steepness+0.05, 1.0);
            std::cout << "steepness: "<< steepness  << std::endl;
            break;
        case 'w':
            desZ -= 0.5;
            break;
        case 's':
            desZ += 0.5;
            break;
        case 'a':
            desX -= 0.5;
            break;
        case 'd':
            desX += 0.5;
            break;
        case 'Q':
            desY -= 0.5;
            break;
        case 'q':
            desY += 0.5;
            break;
        case 'r':
            rotX += 1.0;
            break;
        case 'R':
            rotX -= 1.0;
            break;
  
        case 'm':
            wireframeMode = (wireframeMode == true) ? false : true;
            break;
        // Helicoptero 1
        case 'u':
            desH1X += 0.5;
            break;           
        case 'j':
            desH1X -= 0.5;
            break;
        case 'h':
            desH1Z += 0.5;
            break;
        case 'k':
            desH1Z -= 0.5;
            break;
        case 'y':
            rotHelicoptero += 3.0;
            break;   
        case 'Y':
            rotHelicoptero -= 3.0;
            break;
        case 'o':
            desH1Y += 0.5;
            break;   
        case 'O':
            desH1Y -= 0.5;
            break;
        // Helicoptero 2
        case 'g':
            rotHelicoptero2 += 3.0;
            break;   
        case 'G':
            rotHelicoptero2 -= 3.0;
            break;  
        case 'b':
            if(rotPuertasB < 0.0){
                rotPuertasB += 3.0;
            }
            
            break;   
        case 'B':
            if(rotPuertasB > -45.0){
               rotPuertasB -= 3.0; 
            }           
            break;
        case 'n':
            if(rotPuertasA < 45.0){
                rotPuertasA += 3.0;
            }
            
            break;   
        case 'N':
            if(rotPuertasA > 0){
               rotPuertasA -= 3.0; 
            }
            
            break;
        case 'p':
            desH2Y += 0.5;
            break;   
        case 'P':
            desH2Y -= 0.5;
            break;
        //Luces        
        case 'i':
            if(intensidad > 0){
              intensidad -= 0.1;
              lightDirectional[0].diffuse     = glm::vec3( intensidad, intensidad, intensidad);
              lightDirectional[0].specular    = glm::vec3( intensidad, intensidad, intensidad);  
            }
            
            break;
        case 'I':
            if(intensidad < 1){
               intensidad += 0.1;
               lightDirectional[0].diffuse     = glm::vec3( intensidad, intensidad, intensidad);
               lightDirectional[0].specular    = glm::vec3( intensidad, intensidad, intensidad); 
            }
            
            break; 
        case 'f':
             lightFocal[0].position = glm::vec3(glm::rotate(I, glm::radians(rotFocal), glm::vec3(0.0, 1.0, 0.0)) * glm::vec4(lightFocal[0].position, 0.0));
             lightFocal[0].direction = -lightFocal[0].position;
            break;
        case 'L':
            if(!turnOff){
               for(int i=0; i<N_PLIGHTS; i++){ 
                  lightPositional[i].diffuse     = glm::vec3(0.0, 0.0, 0.0);
                  lightPositional[i].specular    = glm::vec3(0.0, 0.0, 0.0);
                }
               turnOff = true;
            }else{
                for(int i=0; i<N_PLIGHTS; i++){ 
                  lightPositional[i].diffuse     = glm::vec3(0.9, 0.9, 0.9);
                  lightPositional[i].specular    = glm::vec3(0.9, 0.9, 0.9);
                }
               turnOff = false;
            }
            
            break;  
        //Camara
        case 'x':
            X += 1.0;
            break;   
        case 'X':
            X -= 1.0;
            break;
        case 'z':
            Z += 1.0;
            break;   
        case 'Z':
            Z -= 1.0;
            break;
        case 'c':
            Y += 1.0;
            break;   
        case 'C':
            Y -= 1.0;
            break;    
        case 'e':
            auto start = std::chrono::high_resolution_clock::now();
            applyTerrainErosion(1000);
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<float> duration = end - start;
            std::cout << "t erosion: "<< duration.count() << "s " << std::endl;
            terrainModel.updateTerrainModel(t);
            break;
           
           
    }
    
    glutPostRedisplay();

}

void funSpecial(int key, int x, int y) {
   
    switch(key) {
        case GLUT_KEY_UP:    
            desH2X += 0.5;
            break;
        case GLUT_KEY_DOWN:  
            desH2X -= 0.5;
            break;
        case GLUT_KEY_LEFT:  
            desH2Z += 0.5;
            break;
        case GLUT_KEY_RIGHT: 
            desH2Z -= 0.5;
            break;

    }
    glutPostRedisplay();
    
}


/// Implementacion de funciones ------------------------------------------------

void setLights(glm::mat4 P, glm::mat4 V){
    
    shaders.setLight("uLightGlobal",lightGlobal);
    for(int i=0; i<N_DLIGHTS; i++){ 
        shaders.setLight("uLightDirectional["+toString(i)+"]",lightDirectional[i]);   
    }
    for(int i=0; i<N_PLIGHTS; i++){ 
        shaders.setLight("uLightPositional["+toString(i)+"]",lightPositional[i]);   
    }
    
    for(int i=0; i<N_FLIGHTS; i++){ 
        shaders.setLight("uLightFocal["+toString(i)+"]",lightFocal[i]);   
    }
    
    for(int i=0; i<N_PLIGHTS; i++) {
        glm::mat4 M = glm::scale(glm::translate(I,lightPositional[i].position),glm::vec3(0.5));
        drawObjectMat(esfera,matPerl,P,V,M);
    }
    
    for(int i=0; i<N_FLIGHTS; i++) {
        glm::mat4 M = glm::scale(glm::translate(I,lightFocal[i].position),glm::vec3(0.5));
        drawObjectMat(esfera,matPerl,P,V,M);
    }
    

}



void drawObjectMat(Model model, Material material, glm::mat4 P, glm::mat4 V, glm::mat4 M) {
    
    shaders.setMat4("uN"  , glm::transpose(glm::inverse(M)));
    shaders.setMat4("uM"  , M);
    shaders.setMat4("uPVM", P * V * M);
    shaders.setBool("uTerrainShading", false);
    shaders.setBool("uWithMaterials",true);
    
    if(wireframeMode){ 
    glEnable(GL_POLYGON_OFFSET_FILL);
    }
    shaders.setBool("uWireframeMode", false);
    shaders.setMaterial("uMaterial", material);
    model.renderModel(GL_FILL);
    if(wireframeMode){
    glDisable(GL_POLYGON_OFFSET_FILL);
    }
  
    if(wireframeMode){
    shaders.setBool("uWireframeMode", true);
    model.renderModel(GL_LINE);
    }
 
}

void drawObjectTex(Model model, Textures textures, glm::mat4 P, glm::mat4 V, glm::mat4 M) {
    
    shaders.setMat4("uN"  , glm::transpose(glm::inverse(M)));
    shaders.setMat4("uM"  , M);
    shaders.setMat4("uPVM", P * V * M);
    shaders.setBool("uTerrainShading", false);
    shaders.setBool("uWithMaterials",false);
    
    if(wireframeMode){ 
    glEnable(GL_POLYGON_OFFSET_FILL);
    }
    shaders.setBool("uWireframeMode", false);
    shaders.setBool("uNormalMap",textures.normal !=0 ? true : false);
    shaders.setTextures("uTextures",textures);
    model.renderModel(GL_FILL);
    if(wireframeMode){
    glDisable(GL_POLYGON_OFFSET_FILL);
    }
  
    if(wireframeMode){
    shaders.setBool("uWireframeMode", true);
    model.renderModel(GL_LINE);
    }
 
}

void drawTerrain(Model model, Textures texFlat,Textures texSteep, glm::mat4 P, glm::mat4 V, glm::mat4 M){

    shaders.setMat4("uN"  , glm::transpose(glm::inverse(M)));
    shaders.setMat4("uM"  , M);
    shaders.setMat4("uPVM", P * V * M);
    shaders.setBool("uTerrainShading", true);
    shaders.setVec3("uGrassColor", glm::vec3(0.27, 0.34, 0.17));
    shaders.setVec3("uRockColor" , glm::vec3(0.589,0.505,0.421));
    shaders.setVec3("uSnowColor" , glm::vec3(1.0,1.0,1.0));
    shaders.setFloat("uSteepness", steepness);

    if(wireframeMode){ 
    glEnable(GL_POLYGON_OFFSET_FILL);
    }
    shaders.setBool("uWireframeMode", false);
    shaders.setBool("uNormalMapFlat",texFlat.normal !=0 ? true : false);
    shaders.setBool("uNormalMapSteep",texSteep.normal !=0 ? true : false);
    shaders.setTextures("uTexturesFlat",texFlat);
    shaders.setTextures("uTexturesSteep",texSteep);
    model.renderModel(GL_FILL);
    if(wireframeMode){
    glDisable(GL_POLYGON_OFFSET_FILL);
    }
  
    if(wireframeMode){
    shaders.setBool("uWireframeMode", true);
    model.renderModel(GL_LINE);
    }
     
}


void applyTerrainErosion(int nIterations){

    std::cout << "Generando erosion..." << std::endl;
    for(int i=0; i<nIterations; i++){       
        //      minVol,  dt, density,  friction, depositionRate, evapRate
        t.erode(0.01  , 1.2,     1.0,      0.05,       0.01,       0.001);
    }
    //hay que recalcular las normales del terreno modificado
    t.recomputeNormals();
    std::cout << "Erosion generada!" << std::endl;
     
}


//Helicoptero 1
void drawAspa(glm::mat4 P, glm::mat4 V, glm::mat4 M) {
    
    glm::mat4 S = glm::scale(I, glm::vec3(0.37216146, 0.514982975, 0.111648439));
    glm::mat4 T = glm::translate(I, glm::vec3(0.0, -2.75455951691, 0.0));
    glm::mat4 Rx90 = glm::rotate(I, glm::radians(90.f), glm::vec3(1.0, 0.0, 0.0));
    drawObjectMat(cono, matRuby, P, V, M*Rx90*S*T); 
    glm::mat4 T2 = glm::translate(I, glm::vec3(0.0, -1, 0.0));
    glm::mat4 S2 = glm::scale(I, glm::vec3(1.0, 6, 0.3));
    glm::mat4 T3 = glm::translate(I, glm::vec3(0.0, 0.0, -2.5));
    drawObjectMat(cilindro, matGold, P, V, M*T3*Rx90*S2*T2);
}

void drawHelice(glm::mat4 P, glm::mat4 V, glm::mat4 M){
    glm::mat4 Ry90 = glm::rotate(I, glm::radians(90.f), glm::vec3(0.0, 1.0, 0.0));
    drawAspa(P,V,M);
    drawAspa(P,V,M*Ry90);
    drawAspa(P,V,M*Ry90*Ry90);
    drawAspa(P,V,M*Ry90*Ry90*Ry90);
    //Soporte para la helice
    glm::mat4 T = glm::translate(I, glm::vec3(0.0, -1, 0.0));
    glm::mat4 S = glm::scale(I, glm::vec3(1.0, 0.75, 1.0));
    drawObjectMat(cilindro, matGold, P, V, M*S*T);
}

void drawCuerpo(glm::mat4 P, glm::mat4 V, glm::mat4 M){
    glm::mat4 S = glm::scale(I, glm::vec3(4.0, 1.0, 2.25));
    glm::mat4 T = glm::translate(I, glm::vec3(0.0, -7.5, 0.0));
    drawObjectMat(esfera, matRuby, P, V, M*T*S);
    //drawObjectTex(esfera, texHelicopter, P, V, M*T*S);
    glm::mat4 S2 = glm::scale(I, glm::vec3(3.5, 2.0, 2.25));
    glm::mat4 T2 = glm::translate(I, glm::vec3(1.5, -5.5, 0.0));
    drawObjectMat(esfera, matRuby, P, V, M*T2*S2);
   
}

void drawCabina(glm::mat4 P, glm::mat4 V, glm::mat4 M){
    glm::mat4 S = glm::scale(I, glm::vec3(2.5, 2.0, 2.25));
    glm::mat4 T = glm::translate(I, glm::vec3(-2.5, -5.5, 0.0));
    drawObjectMat(esfera, matBronze, P, V, M*T*S);
}


void drawCola(glm::mat4 P, glm::mat4 V, glm::mat4 M){
    glm::mat4 T = glm::translate(I, glm::vec3(0.0, 1.0, 0.0));
    glm::mat4 S = glm::scale(I, glm::vec3(1.0, 10.0, 1.0));
    glm::mat4 RzN90 = glm::rotate(I, glm::radians(-90.f), glm::vec3(0.0, 0.0, 1.0));
    glm::mat4 T2 = glm::translate(I, glm::vec3(0.0, -5.5, 0.0));
    //Dibujamos la cola
    drawObjectMat(cilindro, matGold, P, V, M*T2*RzN90*S*T);
    
    glm::mat4 S2 = glm::scale(I, glm::vec3(0.75, 0.5, 0.75));
    glm::mat4 T3 = glm::translate(I, glm::vec3(18.0, -5.5, 2.0));
    glm::mat4 RxN90 = glm::rotate(I, glm::radians(-90.f), glm::vec3(1.0, 0.0, 0.0));
    //Soporte para las helices de la cola
    drawObjectMat(cilindro, matGold, P, V, M*T3*RxN90*S2*T);
    
    //Rotacion de las helices
    glm::mat4 Ry = glm::rotate(I, glm::radians(rotHelices2), glm::vec3(0.0, 1.0, 0.0));
    //Helices de la cola
    drawHelice2(P,V,M*T3*RxN90*Ry);
}

void drawAspa2(glm::mat4 P, glm::mat4 V, glm::mat4 M){
    
    glm::mat4 S = glm::scale(I, glm::vec3(0.37216146, 0.514982975, 0.111648439));
    glm::mat4 T = glm::translate(I, glm::vec3(0.0, -2.75455951691, 0.0));
    glm::mat4 Rx90 = glm::rotate(I, glm::radians(90.f), glm::vec3(1.0, 0.0, 0.0));
    drawObjectMat(cono, matRuby, P, V, M*Rx90*S*T);
}

void drawHelice2(glm::mat4 P, glm::mat4 V, glm::mat4 M){
    glm::mat4 Ry120 = glm::rotate(I, glm::radians(120.f), glm::vec3(0.0, 1.0, 0.0));
    
    drawAspa2(P,V,M);
    drawAspa2(P,V,M*Ry120);
    drawAspa2(P,V,M*Ry120*Ry120);
}


void drawBase(glm::mat4 P, glm::mat4 V, glm::mat4 M){
    glm::mat4 T = glm::translate(I, glm::vec3(0.0, 1.0, 0.0));
    glm::mat4 S = glm::scale(I, glm::vec3(0.5, 8.0, 1.0));
    glm::mat4 RzN90 = glm::rotate(I, glm::radians(-90.f), glm::vec3(0.0, 0.0, 1.0));
    
    glm::mat4 T1 = glm::translate(I, glm::vec3(-8.0, -9.5, 4.5));
    drawObjectMat(cilindro, matGold, P, V, M*T1*RzN90*S*T);
    
    glm::mat4 T2 = glm::translate(I, glm::vec3(-8.0, -9.5, -4.5));
    drawObjectMat(cilindro, matGold, P, V, M*T2*RzN90*S*T);
    
    glm::mat4 RxN45 = glm::rotate(I, glm::radians(-45.f), glm::vec3(1.0, 0.0, 0.0));
    glm::mat4 Rx45 = glm::rotate(I, glm::radians(45.f), glm::vec3(1.0, 0.0, 0.0));
    glm::mat4 S1 = glm::scale(I, glm::vec3(1.0, 1.505, 0.5));
    
    glm::mat4 T3 = glm::translate(I, glm::vec3(-2.5, -9.5, 4.5));
    drawObjectMat(cilindro, matGold, P, V, M*T3*RxN45*S1*T);
    glm::mat4 T4 = glm::translate(I, glm::vec3(2.5, -9.5, 4.5));
    drawObjectMat(cilindro, matGold, P, V, M*T4*RxN45*S1*T);
    
    glm::mat4 T5 = glm::translate(I, glm::vec3(-2.5, -9.5, -4.5));
    drawObjectMat(cilindro, matGold, P, V, M*T5*Rx45*S1*T);
    glm::mat4 T6 = glm::translate(I, glm::vec3(2.5, -9.5, -4.5));
    drawObjectMat(cilindro, matGold, P, V, M*T6*Rx45*S1*T);
    
    
}



void drawHelicoptero(glm::mat4 P, glm::mat4 V, glm::mat4 M){
    //Rotacion de las helices
    glm::mat4 R = glm::rotate(I, glm::radians(rotHelices1), glm::vec3(0.0, 1.0, 0.0));
    drawHelice(P,V,M*R);
    drawCuerpo(P,V,M);
    drawCola(P,V,M);
    drawBase(P,V,M);

}

//Helicoptero 2

void drawPuertasA(glm::mat4 P, glm::mat4 V, glm::mat4 M){
    //Puertas A
    //Rotacion de las puertas
     glm::mat4 R = glm::rotate(I, glm::radians(rotPuertasA), glm::vec3(0.0, 1.0, 0.0));
     glm::mat4 S = glm::scale(I, glm::vec3(0.017, 0.027, 0.017));
     glm::mat4 T = glm::translate(I, glm::vec3(-2.5, 0.0, 3.5));
     //drawObjectMat(puertasA, matGold, P, V, M*T*R*S);
     drawObjectTex(puertasA, texDoors, P, V, M*T*R*S);
     
     glm::mat4 T2 = glm::translate(I, glm::vec3(-2.5, 0.0, 5.5));
     //drawObjectMat(puertasA, matGold, P, V, M*T2*R*S);
     drawObjectTex(puertasA, texDoors, P, V, M*T2*R*S);
}

void drawPuertasB(glm::mat4 P, glm::mat4 V, glm::mat4 M){
    //Puertas B
    //Rotacion de las puertas
     glm::mat4 R = glm::rotate(I, glm::radians(rotPuertasB), glm::vec3(0.0, 1.0, 0.0));
    
     glm::mat4 S = glm::scale(I, glm::vec3(0.017, 0.027, 0.02));
     glm::mat4 T = glm::translate(I, glm::vec3(2.1, 0.0, 3.5));
     //drawObjectMat(puertasB, matGold, P, V, M*T*R*S);
     drawObjectTex(puertasB, texDoors, P, V, M*T*R*S);
     
     glm::mat4 T2 = glm::translate(I, glm::vec3(2.1, 0.0, 5.5));
     //drawObjectMat(puertasB, matGold, P, V, M*T2*R*S);
     drawObjectTex(puertasB, texDoors, P, V, M*T2*R*S);
}

void drawHeliceHelicoptero2(glm::mat4 P, glm::mat4 V, glm::mat4 M){
    //Helice
    glm::mat4 S = glm::scale(I, glm::vec3(0.03, 0.03, 0.03));
    glm::mat4 T = glm::translate(I, glm::vec3(0.0, 6.0, 0.0));
    //drawObjectMat(helicopteroHelice, matGold, P, V, M*T*S);
    drawObjectTex(helicopteroHelice, texDoors, P, V, M*T*S);
}

void drawCristal(glm::mat4 P, glm::mat4 V, glm::mat4 M){
    drawObjectTex(cristal, texGlass, P, V, M);
}


void drawHelicoptero2(glm::mat4 P, glm::mat4 V, glm::mat4 M){
    
    //Cuerpo
    //drawObjectMat(helicopteroCuerpo, matRuby, P, V, M);
    drawObjectTex(helicopteroCuerpo, texHelicopter, P, V, M);

    drawPuertasA(P,V,M);    
    drawPuertasB(P,V,M);

    glm::mat4 R = glm::rotate(I, glm::radians(rotHelices), glm::vec3(0.0, 1.0, 0.0));
    drawHeliceHelicoptero2(P,V,M*R);
       
     
}

//Plataforma

void drawPlataforma(glm::mat4 P, glm::mat4 V, glm::mat4 M) {

    glm::mat4 S = glm::scale(I, glm::vec3(16.0, 1.0, 16.0));
   drawObjectTex(plataforma, texHelipuerto, P, V, M * S);
}

void funTimer(int ignore) {
    if(rotarHelices){
        rotHelices1 -= 5.0;
        rotHelices2 -= 5.0;
    }
    
    if(rotarHelices2){
        rotHelices -= 5.0;
    }
        
    
    glutPostRedisplay();
    glutTimerFunc(speed, funTimer, 0);

}

