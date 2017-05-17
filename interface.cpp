#include "interface.h"
#include "textures.h"

Interface::Interface() {
}

void Interface::initFont() {
    QImage font;
    font.load("textures/fonts.bmp");
    if (!font.isNull()) // is correct load texture?
    {
        font=QGLWidget::convertToGLFormat(font);
        glGenTextures(1, &fonts);
        glBindTexture(GL_TEXTURE_2D, fonts);
        glTexImage2D(GL_TEXTURE_2D, 0, 3, (GLsizei)font.width(), (GLsizei)font.height(), 
                     0, GL_RGBA, GL_UNSIGNED_BYTE, font.bits());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // задана линейная фильтрация вблизи
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // задана линейная фильтрация вдали
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    }

    for(int i=0; i<6; i++)
        for(int j=0; j<16; j++)
            coords.insert({' ' + i * 16 + j,{j*WTexture, i*HTexture}});
}

void Interface::initTextures() {
    for (std::pair<std::string, GLuint&>& pair : Textures::texturesPaths) {
        QImage texture(pair.first.c_str());
        if (texture.isNull()) { // is correct load texture?
            continue;
        }
        texture=QGLWidget::convertToGLFormat(texture);
        glGenTextures(1, &pair.second);
        glBindTexture(GL_TEXTURE_2D, pair.second);
        glTexImage2D(GL_TEXTURE_2D, 0, 4, (GLsizei)texture.width(), (GLsizei)texture.height(), 
                     0, GL_RGBA, GL_UNSIGNED_BYTE, texture.bits());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // задана линейная фильтрация вблизи
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // задана линейная фильтрация вдали
        glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
    }
}

void Interface::drawText(QString text1, float x, float y, float r, float g, float b)
{
    std::string text = text1.toLocal8Bit().data();
    float dx = 0;
    glBindTexture(GL_TEXTURE_2D, fonts);
    glColor3d(r, g, b);
    glBegin(GL_QUADS);
    for(size_t i=0; i<text.length(); i++)
    {
        glTexCoord2f(coords[text[i]].first, 1 - coords[text[i]].second);
        glVertex2f(x + (i*0.5) + dx, y);
        glTexCoord2f(coords[text[i]].first + WTexture, 1 - coords[text[i]].second);
        glVertex2f((x + (i*0.5) + 0.75 + dx), y);
        glTexCoord2f(coords[text[i]].first + WTexture, 1 - coords[text[i]].second - HTexture);
        glVertex2f((x + (i*0.5) + 0.75 + dx), (y + 0.75));
        glTexCoord2f(coords[text[i]].first, 1 - coords[text[i]].second - HTexture);
        glVertex2f(x + (i*0.5) + dx, (y + 0.75));

        switch(text[i])
        {
        case 'I':
        case ':':
           // dx -= 0.15;
            break;
        case 'W':
            dx += 0.2;
            break;
        case 'M':
        case 'O':
            dx += 0.12;
            break;
        case 'D':
        case 'H':
        case 'A':
        case 'S':
        case 'T':
            dx += 0.05;
            break;
        }
    }
    glEnd();
    glColor3d(1, 1, 1);
}

void Interface::drawDownPanel() {
    isGameOver ? drawText(QString::fromUtf8("GAME OVER"),1,WORLD_LOOK_MAX.i-4.5, 1, 1, 0) :
                 drawText(QString::fromUtf8("TIME LEFT %1").arg(remainTime),1,WORLD_LOOK_MAX.i-4.5, 1, 1, 0);
    drawText(QString::fromUtf8("ENEMY LEFT %1").arg(me->enemyCount),1,WORLD_LOOK_MAX.i-3.75, 1, 1, 0);
    drawText(QString::fromUtf8("HEALTH: %1/%2").arg(me->getHp()).arg(me->getMaxHp()),1,WORLD_LOOK_MAX.i-5.25, 1, 0, 0);
    if (me->target != nullptr) {
        drawText(me->target->getName(),WORLD_LOOK_MAX.j - 10,WORLD_LOOK_MAX.i-6);
        if (me->target->getId() == 1) {
            drawText(QString::fromUtf8("HEALTH: %1").arg(static_cast<Enemy*>(me->target)->getHp()),
                     WORLD_LOOK_MAX.j - 10,WORLD_LOOK_MAX.i-5.25);
        }
    }
}
