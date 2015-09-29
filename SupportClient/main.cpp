#include "frmMain.h"
#include "frmLogin.h"
#include <QApplication>
#include <QTextCodec>
#include <QFileDialog>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTextCodec *codec = QTextCodec::codecForName("UTF8");
    QTextCodec::setCodecForLocale(codec);

    FrmMain fMain;

    QString dbName = QApplication::applicationDirPath() + "/db/support.db";
    if (!QFileInfo::exists(dbName)) {
        dbName = QFileDialog::getOpenFileName(0, QObject::tr("Open file"),
                                              QDir::currentPath() + "/db");
    }
    if (dbName.isEmpty()) {
        QMessageBox::warning(0, QObject::tr("Error"),
                             QObject::tr("Without the database, work impossible"));
        return 0;
    }

    FrmLogin *fLogin = new FrmLogin(0, &fMain);
    fLogin->setDbName(dbName);
    if (!fLogin->exec()) {
        return 0;
    }

    fMain.initDB(dbName);
    fMain.show();

    return a.exec();
}
