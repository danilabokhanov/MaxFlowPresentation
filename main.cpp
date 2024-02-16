#include <QApplication>
#include "application.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    max_flow_app::Application app;
    return a.exec();
}
