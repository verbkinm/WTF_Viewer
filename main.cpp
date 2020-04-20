#include <QApplication>
#include "mainwindow.h"
//#include "menu/filesselectgui.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
//    FilesSelectGUI w("");
    w.showMaximized();

    return a.exec();
}
