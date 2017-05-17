#pragma once
#include <qopengl.h>
#include <utility>
#include <string>

namespace Textures {
    extern std::string heroPath [4];
    extern std::string silverBrickPath;
    extern std::string redBrickPath;
    extern std::string enemyPath;
    extern std::string hpPotionPath;
    extern std::string mpPotionPath;
    //extern std::string fontPath;
    
    extern GLuint hero[4];
    extern GLuint enemy;
//    extern GLuint font;
    
    extern std::pair<std::string, GLuint&> texturesPaths[9];
    extern GLuint textures[4];
}
