#include "mainwindow.hpp"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    wndx::attila::MainWindow w;
    w.show();
    return a.exec();
}
