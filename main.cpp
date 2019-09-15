#include <QApplication>

#include "mainwindow.h"
//#include "graph/centralwidget`.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
//    w.show();

//    CentralWidget w;
//    std::map<double, double> map;
//    map[0] = 0;
//    map[1] = 1;
//    map[2] = 2;
//    map[3] = 3;
//    map[4] = 4;

//    w.addSeries(map, "Test");
    w.showMaximized();

    return a.exec();
    // !!!
}
