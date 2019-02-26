#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    Logger::Instance().add(std::make_shared<ConsoleChannel>("stdout", LTrace));
    w.show();

    return a.exec();
}
