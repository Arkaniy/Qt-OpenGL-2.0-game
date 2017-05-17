#include "gameplay.h"

// игровой мир
Cell world[H / CELL * NDISPLAYS][W / CELL * NDISPLAYS] = {0, false, nullptr};
bool isGameOver = false;

Game::Game()
{
    isGameOver = false;
    new Map();

    ////////////////////////////////////////////////
    // для тестов убрать стены и открыть карту
    for (int i = 0; i<H / CELL * NDISPLAYS; i++)
        for (int j = 0; j<W / CELL * NDISPLAYS; j++)
        {
            //world[i][j].state=1;
            //world[i][j].wasHere=true;
        }
    ////////////////////////////////////////////////

    me = new MainHero();
    for (int i=0; i<50; i++) {
        new Enemy(1);
        new Objects(2);
    }
    for (int i=0; i<5; i++) {
        new Objects(3);
    }
}

int Game::fps()
{
    static int FPS = 0;
    static int count = 0;
    static std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
    std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();

    count++;
    std::chrono::duration<double> dur = end - start;
    if(dur.count() >= 1)
    {
        FPS = count;
        count=0;
        start = end;
    }
    return FPS;
}
