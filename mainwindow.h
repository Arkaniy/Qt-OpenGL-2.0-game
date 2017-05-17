#ifndef SCENE3D_H
#define SCENE3D_H

#include "interface.h"
#include <thread>

class MainWindow : public Interface
{
    Q_OBJECT
private:
    enum Scrolling {none, top, down, left, right,
                    topLeft, topRight, downLeft, downRight} scrolling;  // перемещение камеры
    QTimer timer;
    QTimer timerMoving;
    bool isGrid;        // отображение сетки
    bool isFollowing;   // следование за персонажем
    int dx, dy;         // смещение камеры

    bool isMovingLeft;
    bool isMovingRight;
    bool isMovingUp;
    bool isMovingDown;
    std::thread timerLeft;
public:
    MainWindow();
    void initializeGL();
    void resizeGL(int, int);
    void paintGL();
    void keyPressEvent(QKeyEvent*);
    void keyReleaseEvent(QKeyEvent*);
    void mousePressEvent(QMouseEvent*);
    void mouseMoveEvent(QMouseEvent*);
    void scroll();      // перемещение камеры
    void drawMainScene();
private slots:
    void move();
    void display();
};

#endif // SCENE3D_H
