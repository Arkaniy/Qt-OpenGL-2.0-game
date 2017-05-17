#pragma once

#include "gameplay.h"
#include <map>

class Interface : public Game, public QGLWidget
{
private:
    const float WTexture = float(1) / float(16);
    const float HTexture = float(1) / float(16);
    std::map<char, std::pair<float, float>> coords;
public:
    Interface();
    void initFont();
    void initTextures();
    GLuint fonts;
    void drawText(QString, float, float, float r = 1, float g = 1, float b = 1);
protected:
    void drawDownPanel();
    int remainTime;
};
