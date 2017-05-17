#include "gameplay.h"

Enemy::Enemy(int id) : Units::Units(id)
{
    looking = 10;
    health = maxHealth = 30;
}

void Enemy::draw()
{
    glColor4d(1, getHpRatio(), getHpRatio(), 1);
    glBindTexture(GL_TEXTURE_2D, Textures::enemy);
}

Enemy::~Enemy()
{

}
