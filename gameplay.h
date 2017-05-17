#pragma once

#include <mutex>
#include <QTimer>
#include <vector>
#include <QGLWidget>
#include <QtOpenGL>
#include "textures.h"

struct Point {   // координаты предметов в мире
    int i;
    int j;
};

const int W = 1366;         // ширина поля/окна
const int H = 768;          // высота поля/окна
const int CELL = 40;        // размер клетки
const int NDISPLAYS = 3;    // размер мира в дисплеях О_о

const Point WORLD_LOOK_MAX = { H / CELL, W / CELL};     // максимальные координаты экрана в мировой системе
const Point WORLD_MAX = { WORLD_LOOK_MAX.i * NDISPLAYS, WORLD_LOOK_MAX.j * NDISPLAYS };  // макс. координаты в мире


class AllObjects {   // базовый класс для всех предметов и юнитов
protected:
    QString name;
    Point coord;
    int id;
public:
    AllObjects(int);
    QString getName();
    Point getCoord();
    int getId();
    void deleteObject(AllObjects*);
    virtual void draw() = 0;
    virtual ~AllObjects();
};

class Units : public AllObjects     // базовый класс для всех юнитов
{
private:
    bool isMoving;  // движется юнит или нет
    std::mutex locking; // блокировка данных в потоке
    void searchWayP(int, int);  // обертка для запуска движения в отдельном потоке
protected:
    int looking;        // радиус обзора
    int health;         // текущие очки жизни
    int maxHealth;      // максимальная жизнь
    int currentFrame;   // текущий кадр
public:
    AllObjects *target; // выбранный предмет
    int enemyCount;
    Units(int);
    int getHp();
    int getMaxHp();
    void changeHp(AllObjects*, int);
    double getHpRatio();        // отношение текущих жизней к максимальным, для индикатора
    void action(AllObjects*);   // взаимодействие с другим объектом
    void move(int, int);        // передвижение на 1 клетку
    void searchWay(int, int);   // поиск пути
    bool isSee(int, int);       // есть видимость клетки или нет
    virtual void draw() = 0;
    virtual ~Units();
};

class MainHero : public Units   // класс, описывающий ГГ
{
public:
    MainHero();
    virtual void draw();
    ~MainHero();
private:
    int count;
};

class Enemy : public Units   // класс, описывающий врагов
{
public:
    Enemy(int);
    virtual void draw();
    ~Enemy();

};

class Objects : public AllObjects   // базовый класс для всех объектов
{
public:
    Objects(int);
    virtual void draw();
    ~Objects();
};

class Game  // базовый класс для интерфейса и игровой механики
{
public:
    Game();
protected:
    MainHero *me;   // собственно ГГ   
    int fps();      // подсчет фпс
};

class Map   // карта мира
{
private:
    const int NROOMS = 10; // rooms number
    struct Room
    {
        int i, j, h, w;
        int ic, jc; // middle point
    };
    std::vector<Room> rooms;
    
    Room createRoom();                              // создание комнаты
    bool isIntersection(Room&);                     // проверка на пересечения
    void deleteWasteWalls();                        // удаление лишних стен
    void generatePassage(const Room&, const Room&); // создание корридоров
public:
    Map();
    void newMap();
    ~Map();
};

extern bool isGameOver;
extern struct Cell      // содержимое каждой клетки
{
    char state;         // состояние клетки: проходимость
    bool wasHere;       // флаг посещения клетки
    AllObjects *object; // адрес объекта или юнита
} world[H / CELL * NDISPLAYS][W / CELL * NDISPLAYS];
