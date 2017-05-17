#include "mainwindow.h"
#include "thread"
MainWindow::MainWindow()
{
    // parameters
    setFormat(QGLFormat(QGL::DoubleBuffer));
    setMouseTracking(true);
    // looking
    isGrid = false;
    isFollowing = false;
    isMovingDown = false;
    isMovingLeft = false;
    isMovingRight = false;
    isMovingUp = false;
    remainTime = 90;
    // установка камеры на ГГ
    dx = (me->getCoord().j - (WORLD_LOOK_MAX.j) / 2);
    dy = (me->getCoord().i - (WORLD_LOOK_MAX.i) / 2);

    // main loop
    connect(&timer, SIGNAL(timeout()), this, SLOT(display()));
    connect(&timerMoving, SIGNAL(timeout()), this, SLOT(move()));
    timer.start(20);
    timerMoving.start(200);
    
//    timerLeft = std::thread([this]{
//        while (remainTime > 0) {
//            std::this_thread::sleep_for(std::chrono::seconds(1));
//            remainTime--;
//        } 
//        isGameOver = true;
//    });
}

void MainWindow::initializeGL() {
    glClearColor(0, 0, 0, 1);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    initFont();     // инициализация шрифта
    initTextures();
}

void MainWindow::resizeGL(int nWidth, int nHeight) {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glViewport(0, 0, (GLint)nWidth, (GLint)nHeight);
}

void MainWindow::paintGL() {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glOrtho(0,WORLD_LOOK_MAX.j,WORLD_LOOK_MAX.i,0,1,0);
}

void MainWindow::display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3d(1, 1, 1);

    //scrolling
    if(scrolling != Scrolling::none)
        scroll();

    // following
    if (isFollowing) {
        dx = (me->getCoord().j - WORLD_LOOK_MAX.j / 2);
        dy = (me->getCoord().i - WORLD_LOOK_MAX.i / 2);
    }

    drawMainScene();    // отрисовка сцены

    // draw grid
    if(isGrid) {
        glBindTexture(GL_TEXTURE_2D, 0);
        glBegin(GL_LINES);
            glColor3d(0.8, 0.8, 0.8);
            for (int j = 0; j < WORLD_LOOK_MAX.j; j++) {
                glVertex2i(j, 0);
                glVertex2i(j, WORLD_LOOK_MAX.i);
                if(j<=WORLD_LOOK_MAX.i) {
                    glVertex2i(0, j);
                    glVertex2i(WORLD_LOOK_MAX.j, j);
                }
            }
            glColor3d(1, 1, 1);
        glEnd();
    }

    // info
    drawText(QString::fromUtf8("FPS: %1").arg(fps()) , 1, 1, 0, 1, 0);
//    drawText(QString::fromUtf8("UNITS PTR SIZE: %1").arg(sizeof(Units*)) , 1, 1.75, 0, 0, 1);
//    drawText(QString::fromUtf8("UNIT SIZE: %1").arg(sizeof(Units)) , 1, 2.5, 1, 0, 0);
//    drawText(QString::fromUtf8("WORLD SIZE: %1").arg(sizeof(world)) ,1,3.25);
//    drawText(QString::fromUtf8("WORLD CELLS SIZE: %1").arg(sizeof(world[0][0])) ,1,4);

    drawDownPanel();

    this->swapBuffers();
}
    
void MainWindow::keyPressEvent(QKeyEvent *e) {
    switch (e->key()) {
    case Qt::Key_W:
    case Qt::Key_Up:
        isMovingUp = true;
        break;
    case Qt::Key_A:
    case Qt::Key_Left:
        isMovingLeft = true;
        break;
    case Qt::Key_S:
    case Qt::Key_Down:
        isMovingDown = true;
        break;
    case Qt::Key_D:
    case Qt::Key_Right:
        isMovingRight = true;
        break;

    case Qt::Key_Space:
        isFollowing = true;
        break;
    case Qt::Key_F:
        if(me->target != nullptr)
            me->searchWay(me->target->getCoord().i, me->target->getCoord().j);
        break;

    case Qt::Key_F1:
        isGrid = !isGrid;
        break;
    case Qt::Key_F2:
        isFollowing = !isFollowing;
        break;

    case Qt::Key_Escape:
        if(me->target == nullptr) {
            exit(0);
        }
        else
            me->target = nullptr;
        break;
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *e) {
    switch (e->key()) {
    case Qt::Key_Space:
        isFollowing = false;
        break;
    case Qt::Key_W:
    case Qt::Key_Up:
        isMovingUp = false;
        break;
    case Qt::Key_A:
    case Qt::Key_Left:
        isMovingLeft = false;
        break;
    case Qt::Key_S:
    case Qt::Key_Down:
        isMovingDown = false;
        break;
    case Qt::Key_D:
    case Qt::Key_Right:
        isMovingRight = false;
        break;
    }
}

void MainWindow::mousePressEvent(QMouseEvent *e)
{
    if (!isGameOver) {
        int i = (e->y()) / CELL + dy;
        int j = (e->x()-CELL/2) / CELL + dx;
    
        if (world[i][j].state == 1) {
            if (world[i][j].object == nullptr || me->target == world[i][j].object)
                me->searchWay(i, j);
            else
                me->target = world[i][j].object;
        }
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *e) {
    if (e->pos().x() < 5 && e->pos().y() < 5) scrolling = topLeft;
    else if (e->pos().x() < 5 && e->pos().y() > H-5) scrolling = downLeft;
    else if (e->pos().x() > W-5 && e->pos().y() < 5) scrolling = topRight;
    else if (e->pos().x() > W-5 && e->pos().y() > H-5) scrolling = downRight;
    else if (e->pos().x() < 5) scrolling = left;
    else if (e->pos().x() > W-5) scrolling = right;
    else if (e->pos().y() < 5) scrolling = top;
    else if (e->pos().y() > H-5) scrolling = down;
    else scrolling = none;
}

void MainWindow::scroll() {
    switch(scrolling)
    {
    case top:
        if(dy > 0) {
            dy--;
        }
        isFollowing = false;
        break;
    case down:
        if(dy < WORLD_MAX.i - WORLD_LOOK_MAX.i) {
            dy++;
        }
        isFollowing = false;
        break;
    case left:
        if(dx > 0) {
            dx--;
        }
        isFollowing = false;
        break;
    case right:
        if(dx < WORLD_MAX.j - WORLD_LOOK_MAX.j) {
            dx++;
        }
        isFollowing = false;
        break;
    case topLeft:
        if(dx > 0 && dy > 0) {
            dx--;
            dy--;
        }
        isFollowing = false;
        break;
    case topRight:
        if(dx < WORLD_MAX.j - WORLD_LOOK_MAX.j && dy > 0) {
            dx++;
            dy--;
        }
        isFollowing = false;
        break;
    case downLeft:
        if(dx > 0 && dy < WORLD_MAX.i - WORLD_LOOK_MAX.i) {
            dx--;
            dy++;
        }
        isFollowing = false;
        break;
    case downRight:
        if(dx < WORLD_MAX.j - WORLD_LOOK_MAX.j && dy < WORLD_MAX.i - WORLD_LOOK_MAX.i) {
            dx++;
            dy++;
        }
        isFollowing = false;
        break;
    case none:
        ;   // do nothing
    }
}

void MainWindow::drawMainScene() {
    for (int i = 0; i < WORLD_LOOK_MAX.i; i++) {
        for (int j = 0; j < WORLD_LOOK_MAX.j; j++) {
            int iCurr = i + dy;
            int jCurr = j + dx;
            if (!(iCurr < 0 || iCurr >= WORLD_MAX.i || jCurr < 0 || jCurr >= WORLD_MAX.j) &&
                (world[iCurr][jCurr].wasHere || me->isSee(iCurr,jCurr))) {
                if (world[iCurr][jCurr].state) {
                    glBindTexture(GL_TEXTURE_2D, Textures::textures[world[iCurr][jCurr].state - 1]);
                }
                if (!me->isSee(iCurr, jCurr)) {
                    glColor4d(1, 1, 1, 0.25);
                }
                glBegin(GL_QUADS);
                    glTexCoord2f(0.0f, 1.0f); glVertex2i(j, i);
                    glTexCoord2f(1.0f, 1.0f); glVertex2i((j + 1), i);
                    glTexCoord2f(1.0f, 0.0f); glVertex2i((j + 1), (i + 1));
                    glTexCoord2f(0.0f, 0.0f); glVertex2i(j, (i + 1));
                    glColor3d(1, 1, 1);
                glEnd();

                if (world[iCurr][jCurr].object != nullptr) {
                    if (me->isSee(iCurr, jCurr)) {
                        world[iCurr][jCurr].object->draw();
                        glBegin(GL_QUADS);
                            glTexCoord2f(0.0f, 1.0f); glVertex2i(j, i);
                            glTexCoord2f(1.0f, 1.0f); glVertex2i((j + 1), i);
                            glTexCoord2f(1.0f, 0.0f); glVertex2i((j + 1), (i + 1));
                            glTexCoord2f(0.0f, 0.0f); glVertex2i(j, (i + 1));
                            glColor3d(1, 1, 1);
                        glEnd();
                    }
                }
                glBindTexture(GL_TEXTURE_2D, 0);
                world[iCurr][jCurr].wasHere = true;
            }
        }
    }
}

void MainWindow::move() {
    if (!isGameOver) {
        if (isMovingDown) me->move(1, 0);
        if (isMovingUp) me->move(-1, 0);
        if (isMovingRight) me->move(0, 1);
        if (isMovingLeft) me->move(0, -1);
    }
}
