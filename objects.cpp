#include "gameplay.h"
#include "textures.h"

Objects::Objects(int id) : AllObjects(id)
{

}

void Objects::draw() {
    glBindTexture(GL_TEXTURE_2D, Textures::textures[id]);
}

Objects::~Objects()
{

}
