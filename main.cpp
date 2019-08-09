#include <QApplication>

#include "mainwindow.h"

//#include "viewer/viewer.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
//    Viewer w;
//    w.show();

//    for (auto &child : w.children()) {
//        qDebug() << sizeof (child);
//    }
    w.showMaximized();

    return a.exec();
    // !!!
}
