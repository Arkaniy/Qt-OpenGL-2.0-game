#include "gameplay.h"

AllObjects::AllObjects(int id) {
    this->id = id;
    switch (id) {
    case 1:
        name = "TROLL";
        break;
    case 2:
        name = "HEALTH POTION";
        break;
    }

    do {
        coord.i = rand() % WORLD_MAX.i;
        coord.j = rand() % WORLD_MAX.j;
    } while (!(world[coord.i][coord.j].state == 1 && world[coord.i][coord.j].object == nullptr));

    world[coord.i][coord.j].object = this;
}

int AllObjects::getId() {
    return id;
}

Point AllObjects::getCoord() {
    return coord;
}

QString AllObjects::getName() {
    return name;
}

void AllObjects::deleteObject(AllObjects *from) {
    if(from->id == 0)
        dynamic_cast<MainHero*>(from)->target = nullptr;
        if(id == 1) {
            dynamic_cast<MainHero*>(from)->enemyCount--;
        }
    world[getCoord().i][getCoord().j].object = nullptr;
    delete this;
}

AllObjects::~AllObjects() {

}
