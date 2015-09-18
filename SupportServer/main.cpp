#include "frmMain.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    frmMain fMain;
    fMain.show();

    return a.exec();
}
