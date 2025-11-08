#include "mainwindow.hpp"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    wndx::MainWindow w;
    w.show();
    return a.exec();
}
