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

    void setDbName(QString);
    QString dbName();

private:
    Ui::FrmLogin *ui;
    QSqlDatabase m_db;

    FrmMain *m_fMain;

    QString m_dbName;

    bool checkLogin();
    void animationForm();

protected slots:
    void frmClose();
    void frmEnter();
    void animationFinish();
};

#endif // FRMLOGIN_H
