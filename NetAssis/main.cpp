#include "NetAssis.h"
#include <QtWidgets/QApplication>
#include <QDesktopWidget>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    NetAssis w;
    w.show();
    w.setFixedSize(1200, 800);
    w.move((QApplication::desktop()->width() - w.width()) / 2, (QApplication::desktop()->height() - w.height()) / 2);
    return a.exec();
}
