#ifndef FRMLOGIN_H
#define FRMLOGIN_H

#include "frmMain.h"
#include <QDialog>
#include <QSqlDatabase>
#include <QList>

namespace Ui {
class FrmLogin;
}

class FrmLogin : public QDialog
{
    Q_OBJECT

public:
    explicit FrmLogin(QWidget *parent = 0, FrmMain *pFromMain = 0);
    ~FrmLogin();

    QString dbName;


private:
    Ui::FrmLogin *ui;
    QSqlDatabase db;
    QList<QWidget *> listElements;

     FrmMain *fMain;

    bool checkLogin();
    void animationForm();

protected slots:
    void frmClose();
    void frmEnter();
    void animationFinish();
};

#endif // FRMLOGIN_H
