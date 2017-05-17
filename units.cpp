#include "gameplay.h"

#include <queue>
#include <thread>
#include <QThread>

Units::Units(int id) : AllObjects(id) {
    target = nullptr;
    currentFrame = 0;   // поставить на рандом
    enemyCount = 3;
}

Units:: ~Units() {

}

void Units::move(int di, int dj)
{
    isMoving = true;
    if(dj == -1)
    {
        if(currentFrame == 0 )
            currentFrame = 1;
        else
            currentFrame = 0;
    }
    else if(dj == 1)
    {
        if(currentFrame == 2 )
            currentFrame = 3;
        else
            currentFrame = 2;
    }
    if(world[coord.i+di][coord.j+dj].state == 1)
    {
        if(world[coord.i+di][coord.j+dj].object == nullptr)
        {
            world[coord.i][coord.j].object = nullptr;
            coord.i += di;
            coord.j += dj;
            world[coord.i][coord.j].object = this;
        }
        else
        {
            if(world[coord.i+di][coord.j+dj].object != target)
                target = world[coord.i+di][coord.j+dj].object;
        }
    }
}

void Units::searchWay(int iFinish, int jFinish)
{
    if(world[iFinish][jFinish].state != 1)
        return;
    isMoving = true;
    std::thread thr(&Units::searchWayP, this, iFinish, jFinish);
    thr.detach();
}

void Units::searchWayP(int iFinish, int jFinish)
{
    locking.lock(); // mutex lock
    isMoving = false;

    int ways[8][2] = { { 1, 0 }, { 0, 1 }, { -1, 0 }, { 0, -1 }, { 1, 1 }, { 1, -1 }, { -1, 1 }, { -1, -1 } };
    int matrix[H / CELL * NDISPLAYS][W / CELL * NDISPLAYS] = { 0 };
    int count = 1;
    matrix[coord.i][coord.j] = count++;
    bool searchOver = false;
    // search
    while (matrix[iFinish][jFinish] == 0 && !searchOver)
    {
        searchOver = true;  // флаг окончания поиска, защита от зацикливания
        for (int i = 1; i < WORLD_MAX.i - 1; i++) {
            for (int j = 1; j < WORLD_MAX.j - 1; j++) {
                if (matrix[i][j] == count - 1) {
                    for (int k = 0; k < 8; k++) {
                        if (matrix[i + ways[k][0]][j + ways[k][1]] == 0 &&  // проверка по массиву волны: не проходил тут
                        // проходимая и пустая клетка
                        ((world[i + ways[k][0]][j + ways[k][1]].state == 1 && 
                          world[i + ways[k][0]][j + ways[k][1]].object == nullptr) ||
                          ((i + ways[k][0]) == iFinish && (j + ways[k][1]) == jFinish)) &&    // или конечная клетка
                          world[i + ways[k][0]][j + ways[k][1]].wasHere)  // + был там
                        {
                            // ограничение на срез углов у стен
                            if((k>3 && world[i + ways[k][0]][j].state == 1 &&
                                world[i][j + ways[k][1]].state == 1) || k<4) {
                                matrix[i + ways[k][0]][j + ways[k][1]] = count;
                                searchOver = false;
                            }
                        }
                    }
                }
            }
        }
        count++;
    }

    count--;
    int i = iFinish;
    int j = jFinish;

    // come back
    if (!searchOver) {
        std::vector<int> pathBack;
        while (!(coord.i == i && coord.j == j)) {
            for (int k = 0; k < 8; k++) {
                if (matrix[i + ways[k][0]][j + ways[k][1]] == count - 1) {
                    pathBack.push_back(k);
                    i += ways[k][0];
                    j += ways[k][1];
                    count--;
                    break;
                }
            }
        }

        while (!pathBack.empty()) {
            if(isMoving) {
                locking.unlock();
                return;
            }
            int i = pathBack[pathBack.size() - 1];
            pathBack.pop_back();

            move(-ways[i][0], -ways[i][1]);
            isMoving = false;
            QThread::msleep(200);
        }

        if (world[iFinish][jFinish].object == target && target != nullptr)
            action(world[iFinish][jFinish].object);
    }
    locking.unlock(); // mutex unlock
}

bool Units::isSee(int i, int j) {
    if(abs(i - coord.i) + abs(j - coord.j) <= looking &&
            abs(i - coord.i) < looking * 0.75 && abs(j - coord.j) < looking * 0.75) // limit range
    {
        int di = abs(coord.i - i);
        int dj = abs(coord.j - j);
        int e = 0;
        if (di<=dj) {
            int i2 = coord.i;
            for(int j2 = coord.j; (coord.j<=j)?j2 <= j:j2>=j; (coord.j<=j)?j2++:j2--) {
                if(world[i2][j2].state == 2 && !(i2==i && j2==j))
                    return false;
                e += di;
                if (2*e >= dj) {
                    if(coord.i >= i)
                        i2--;
                    else
                        i2++;
                    e -= dj;
                }
            }
        }
        else {
            int j2 = coord.j;
            for(int i2 = coord.i; (coord.i<=i)?i2 <= i:i2>=i; (coord.i<=i)?i2++:i2--) {
                if(world[i2][j2].state == 2 && !(i2==i && j2==j))
                    return false;
                e += dj;
                if (2*e >= di) {
                    if(coord.j >= j)
                        j2--;
                    else
                        j2++;
                    e -= di;
                }
            }
        }
        return true;
    }
    return false;
}

void Units::action(AllObjects *to) {
    switch (to->getId())
    {
    case 1:
    {
        Enemy *enemy = static_cast<Enemy*>(to);
        enemy->changeHp(this, -3);
        this->changeHp(enemy, -4);
        break;
    }
    case 2:
        to->deleteObject(this);
        this->changeHp(this->target, 10);
        break;
    case 3:
        if (enemyCount <= 0) {
            isGameOver = true;
        }
        break;
    default:
        break;
    }

}

void Units::changeHp(AllObjects *from, int dhp) {
    health += dhp;
    if(health>maxHealth)
        health = maxHealth;
    else if(health<=0) {
        if (id == 0) {
            isGameOver = true;
        }
        deleteObject(from);
    }
}

int Units::getHp() {
    return health;
}

int Units::getMaxHp() {
    return maxHealth;
}

double Units::getHpRatio() {
    return static_cast<double>(health)/static_cast<double>(maxHealth);
}
