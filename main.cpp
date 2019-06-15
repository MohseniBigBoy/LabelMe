#include "mainwindow.h"
#include <QApplication>


//Last modified date: Monday, August 13, 2018

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}

