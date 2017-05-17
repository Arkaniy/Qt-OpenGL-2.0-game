#include "gameplay.h"

MainHero::MainHero() : Units::Units(0)
{
    looking = 12;
    health = 35;
    maxHealth = 50;
}

void MainHero::draw() {
    glColor4d(1, getHpRatio(), getHpRatio(), 1);
    glBindTexture(GL_TEXTURE_2D, Textures::hero[currentFrame]);
}

MainHero::~MainHero()
{

}
