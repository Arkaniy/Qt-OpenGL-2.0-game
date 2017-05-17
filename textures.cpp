#include "textures.h"

namespace Textures {
    std::string heroPath [4] =
    {
        "textures/heroLeft1.bmp",
        "textures/heroLeft2.bmp",
        "textures/heroRight1.bmp",
        "textures/heroRight2.bmp"
    };
    std::string silverBrick = "textures/Silver_Brick.png";
    std::string redBrick = "textures/Red_Brick.png";
    std::string enemyPath = "textures/enemy.png";
    std::string hpPotion = "textures/hpPotion.png";
    std::string mpPotion = "textures/mpPotion.png";
//    std::string fontPath = "textures/fonts.bmp";
    
    
    GLuint hero[4];
    GLuint enemy;
    
    GLuint textures[4];
//    GLuint font;
    
    std::pair<std::string, GLuint&> texturesPaths[] {
        { silverBrick,  textures[0] },
        { redBrick,     textures[1] },
        { hpPotion,     textures[2] },
        { mpPotion,     textures[3] },
        { enemyPath,        enemy       },
        { heroPath[0],      hero[0]     },
        { heroPath[1],      hero[1]     },
        { heroPath[2],      hero[2]     },
        { heroPath[3],      hero[3]     }
        //{ fontPath, font}
    };
}
