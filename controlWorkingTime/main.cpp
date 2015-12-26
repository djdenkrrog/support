#include "frmMain.h"
#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QDesktopWidget>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setWindowIcon(QIcon(":images/icon.png"));

    FrmMain frmMain;

    QString dbName = QApplication::applicationDirPath() + "/db/timework.db";
    if (!QFileInfo::exists(dbName)) {
        dbName = QFileDialog::getOpenFileName(0, QObject::tr("Open file"),
                                              QDir::currentPath() + "/db");
    }
    if (dbName.isEmpty()) {
        QMessageBox::warning(0, QObject::tr("Error"),
                             QObject::tr("Without the database, work impossible."));
        return 0;
    }

    if (!frmMain.initDB(dbName)) {
        return 0;
    }

    frmMain.move(QApplication::desktop()->screen()->rect().center() - frmMain.rect().center());

    frmMain.show();

    return a.exec();
}
