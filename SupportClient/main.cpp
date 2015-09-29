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
        dbName = QFileDialog::getOpenFileName(0, "Open file", QDir::currentPath() + "/db");
    }
    if (dbName == "") {
        QMessageBox::warning( 0 , "Error", "Without the database, work impossible");
        return 0;
    }

    FrmLogin *fLogin = new FrmLogin(0, &fMain);
    fLogin->dbName = dbName;
    if (!fLogin->exec()) {
        return 0;
    }

    fMain.initDB(dbName);
    fMain.show();

    return a.exec();
}
