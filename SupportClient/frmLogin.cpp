#include "frmLogin.h"
#include "ui_frmLogin.h"
#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QDebug>
#include <QPropertyAnimation>

frmLogin::frmLogin(QWidget *parent, frmMain *pFromMain) :
    QDialog(parent),
    ui(new Ui::frmLogin)
{
    ui->setupUi(this);

    ui->btnEnter->setIcon(QIcon(":/images/confirm.png"));
    ui->btnCancel->setIcon(QIcon(":/images/cancel.png"));
    ui->edtPassword->setEchoMode(QLineEdit::Password);

    connect(ui->btnCancel, SIGNAL(clicked(bool)), SLOT(frmClose()));
    connect(ui->btnEnter, SIGNAL(clicked(bool)), SLOT(frmEnter()));

    listElements.append(ui->lblUserName);
    listElements.append(ui->edtLogin);
    listElements.append(ui->lblUserPassword);
    listElements.append(ui->edtPassword);
    listElements.append(ui->lblImg);
    listElements.append(ui->btnCancel);

    fMain = pFromMain;
}
//----------------------------------------------------------------------------------------------------------------------

void frmLogin::frmEnter()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbName);
    if (!db.open()) {
        QMessageBox::warning( 0 , tr("Error opening DB!"), db.lastError().databaseText());
    }

    if (checkLogin())
    {
        QSqlQuery query;

        query.prepare("select user_, name1, name2, name3 from users where user_=:user and passw=:passw;");
        query.bindValue(":user", ui->edtLogin->text());
        query.bindValue(":passw", ui->edtPassword->text());

        if (query.exec()) {
            query.next();
            QSqlRecord rec = query.record();
            fMain->userInfo.user = query.value(rec.indexOf("user_")).toString();
            fMain->userInfo.name1 = query.value(rec.indexOf("name1")).toString();
            fMain->userInfo.name2 = query.value(rec.indexOf("name2")).toString();
            fMain->userInfo.name3 = query.value(rec.indexOf("name3")).toString();
        } else {
            QMessageBox::warning( 0 , tr("Error execution sql query!"), query.lastError().driverText());
        }

        animationForm();
    }
}
//----------------------------------------------------------------------------------------------------------------------

bool frmLogin::checkLogin()
{
    static int countTry = 0;
    bool result = false;

    QSqlQuery query;
    query.prepare("select count(user_) as cnt from users where user_=:user and passw=:passw;");
    query.bindValue(":user", ui->edtLogin->text());
    query.bindValue(":passw", ui->edtPassword->text());

    if (query.exec()) {
        query.next();
        QSqlRecord rec = query.record();
        int qSize = query.value(rec.indexOf("cnt")).toInt();
        if (qSize > 0) {
           result = true;
        }
        ++countTry;
        if (countTry == 3 && !result) {
            close();
        } else if (!result) {
            QMessageBox::warning( 0 , tr("Failed password entry!"), QString(tr("Incorrect login or password. \nAttempts remaining: %1")).arg((3 - countTry)));
        }
    } else {
        QMessageBox::warning( 0 , tr("Error execution sql query!"), query.lastError().driverText());
    }

    return result;
}
//----------------------------------------------------------------------------------------------------------------------

void frmLogin::animationForm()
{
    int countElements = listElements.size();
    this->setMinimumSize(0,0);
    for (int i = 0; i < countElements; ++i) {
        QPropertyAnimation *animation = new QPropertyAnimation(listElements.at(i), "geometry");
        animation->setDuration(1500);
        animation->setEasingCurve(QEasingCurve::Linear);
        animation->setEndValue(QRect(ui->btnEnter->geometry().x() + ui->btnEnter->geometry().width() / 2, ui->btnEnter->geometry().y() + ui->btnEnter->geometry().height() / 2, 0, 0));
        animation->start(QAbstractAnimation::DeleteWhenStopped);
    }

    QPropertyAnimation *animation = new QPropertyAnimation(this, "windowOpacity");
    animation->setDuration(1500);
    animation->setEasingCurve(QEasingCurve::Linear);
    animation->setStartValue(1);
    animation->setEndValue(0);
    animation->start();
    connect(animation, SIGNAL(finished()), SLOT(animationFinish()));

}
//----------------------------------------------------------------------------------------------------------------------

void frmLogin::animationFinish()
{
    this->accept();
}

//----------------------------------------------------------------------------------------------------------------------

void frmLogin::frmClose()
{
    this->reject();
}
//----------------------------------------------------------------------------------------------------------------------

frmLogin::~frmLogin()
{
    db.close();
    delete ui;
}
//----------------------------------------------------------------------------------------------------------------------
