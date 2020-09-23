#include "Texture.h"

//------------------------
// Crea la textura
//------------------------
void Texture::initTexture(const char *textureFile) {
    
 // Creamos la textura a configurar
    glGenTextures(1,&texture);  
    glBindTexture(GL_TEXTURE_2D, texture);
    
 // Cargamos la imagen
    unsigned int  w, h;
    unsigned char *pixels = loadTexture(textureFile, w, h);  
    
 // Creamos la textura
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void *)pixels);
    glGenerateMipmap(GL_TEXTURE_2D);
    
 // Configuramos la textura
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    float aniso = 0.0f;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso);

}

//--------------------------------------------------
// Carga una textura mediante la librería Freeimage
//--------------------------------------------------
unsigned char* Texture::loadTexture(const char *textureFile, unsigned int &w, unsigned int &h) {
    
    FreeImage_Initialise(TRUE);

 // Leemos la textura
    FREE_IMAGE_FORMAT format = FreeImage_GetFileType(textureFile,0);
    if(format==FIF_UNKNOWN) format = FreeImage_GetFIFFromFilename(textureFile);
    if((format==FIF_UNKNOWN) || !FreeImage_FIFSupportsReading(format)) {
        std::cout << "Formato de la textura " << textureFile << " no está soportado." << std::endl;
        std::cin.get();
        exit(1);
    }
    FIBITMAP *texture = FreeImage_Load(format,textureFile);
    if(texture==NULL) {
        std::cout << "El fichero " << textureFile << " no se puede abrir." << std::endl;
        std::cin.get();
        exit(1);
    }
    FIBITMAP *temp = texture;
    texture = FreeImage_ConvertTo32Bits(texture);
    FreeImage_Unload(temp);

 // De BGRA a RGBA
    w = FreeImage_GetWidth(texture);
    h = FreeImage_GetHeight(texture);
    unsigned char *pixelsBGRA = (unsigned char *)FreeImage_GetBits(texture);
    unsigned char *pixelsRGBA = new unsigned char[4*w*h];
    for(int j=0; j<w*h; j++){
        pixelsRGBA[j*4+0] = pixelsBGRA[j*4+2];
        pixelsRGBA[j*4+1] = pixelsBGRA[j*4+1];
        pixelsRGBA[j*4+2] = pixelsBGRA[j*4+0];
        pixelsRGBA[j*4+3] = pixelsBGRA[j*4+3];
    }

    FreeImage_Unload(texture);
    FreeImage_DeInitialise();

    return pixelsRGBA;
}

//-----------------------------------------
// Devuelve el identificador de la textura
//-----------------------------------------
unsigned int Texture::getTexture() {
    
    return texture;
    
}

//-----------------------
// Destructor dela clase
//-----------------------
Texture::~Texture() {
    
    glDeleteTextures(1,&texture);
    
}
