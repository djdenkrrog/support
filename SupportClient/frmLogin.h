#ifndef FRMLOGIN_H
#define FRMLOGIN_H

#include "frmMain.h"
#include <QDialog>
#include <QSqlDatabase>
#include <QList>

namespace Ui {
class frmLogin;
}

class frmLogin : public QDialog
{
    Q_OBJECT

public:
    explicit frmLogin(QWidget *parent = 0, frmMain *pFromMain = 0);
    ~frmLogin();

    QString dbName;


private:
    Ui::frmLogin *ui;
    QSqlDatabase db;
    QList<QWidget *> listElements;

     frmMain *fMain;

    bool checkLogin();
    void animationForm();

protected slots:
    void frmClose();
    void frmEnter();
    void animationFinish();
};

#endif // FRMLOGIN_H
