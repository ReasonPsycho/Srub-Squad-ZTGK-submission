#ifndef TEXTURE_H
#define TEXTURE_H


#include "stb_image.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "spdlog/spdlog.h"
#include "glad/glad.h"

using namespace std;

using Color = GLubyte[4];


class Texture {
public:
    Texture(string path, string type); //TODO change this to enum
    Texture( const Color& color = {0, 0, 0, 0}); 

    ~Texture();

    // the texture ID
    GLuint ID{}; // TODO add more than one texture on top of it
    string path;
    string type;
    string name;

    // constructor reads and builds the texture
    void use(GLenum GL_TEXTUREX);
};


#endif 