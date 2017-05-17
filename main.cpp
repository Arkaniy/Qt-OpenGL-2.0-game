#include <QtGui>
#include "mainwindow.h"

int main(int argc, char** argv)
{
    srand(static_cast<unsigned int>(time(0)));
    QApplication app(argc, argv);
    MainWindow window;
    window.setWindowTitle("My game");
    //window.show();
    window.showFullScreen();
    return app.exec();
}
