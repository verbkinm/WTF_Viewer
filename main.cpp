#include <QApplication>

//#include "mainwindow.h"
#include "graph/centralwidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    MainWindow w;
//    w.show();

    CentralWidget w;
    std::vector<QPointF> pointVector;
    pointVector.push_back({0,0});
    pointVector.push_back({1,1});
    pointVector.push_back({2,2});
    pointVector.push_back({3,3});
    pointVector.push_back({4,4});

    w.addSeries(pointVector, "Test");
    w.showMaximized();

    return a.exec();
    // !!!
}
