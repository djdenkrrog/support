#include "frmLogin.h"
#include "ui_frmLogin.h"
#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QDebug>
#include <QPropertyAnimation>

FrmLogin::FrmLogin(QWidget *parent, FrmMain *pFromMain) :
    QDialog(parent),
    ui(new Ui::FrmLogin)
{
    ui->setupUi(this);

    ui->btnEnter->setIcon(QIcon(QStringLiteral(":/images/confirm.png")));
    ui->btnCancel->setIcon(QIcon(QStringLiteral(":/images/cancel.png")));
    ui->edtPassword->setEchoMode(QLineEdit::Password);

    connect(ui->btnCancel, SIGNAL(clicked(bool)), SLOT(frmClose()));
    connect(ui->btnEnter, SIGNAL(clicked(bool)), SLOT(frmEnter()));

    m_listElements.append(ui->lblUserName);
    m_listElements.append(ui->edtLogin);
    m_listElements.append(ui->lblUserPassword);
    m_listElements.append(ui->edtPassword);
    m_listElements.append(ui->lblImg);
    m_listElements.append(ui->btnCancel);

    m_fMain = pFromMain;
}
//--------------------------------------------------------------------------------------------------

void FrmLogin::frmEnter()
{
    m_db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"));
    m_db.setDatabaseName(m_dbName);
    if (!m_db.open()) {
        QMessageBox::warning(this, tr("Error opening DB!"), m_db.lastError().databaseText());
    }

    if (checkLogin()) {
        QSqlQuery query;

        query.prepare(QStringLiteral("select user_, name1, name2, name3 from users \
                                     where user_=:user and passw=:passw;"));
        query.bindValue(QStringLiteral(":user"), ui->edtLogin->text());
        query.bindValue(QStringLiteral(":passw"), ui->edtPassword->text());

        if (query.exec()) {
            query.next();
            QSqlRecord rec = query.record();
            m_fMain->setUserInfo(query.value(rec.indexOf(QStringLiteral("user_"))).toString(),
                               query.value(rec.indexOf(QStringLiteral("name1"))).toString(),
                               query.value(rec.indexOf(QStringLiteral("name2"))).toString(),
                               query.value(rec.indexOf(QStringLiteral("name3"))).toString()
                               );
        } else {
            QMessageBox::warning(this, tr("Error execution sql query!"),
                                 query.lastError().driverText());
        }

        m_db.close();
        animationForm();
    }
}
//--------------------------------------------------------------------------------------------------

bool FrmLogin::checkLogin()
{
    static int countTry = 0;
    bool result = false;

    QSqlQuery query;
    query.prepare(QStringLiteral("select count(user_) as cnt from users \
                                 where user_=:user and passw=:passw;"));
    query.bindValue(QStringLiteral(":user"), ui->edtLogin->text());
    query.bindValue(QStringLiteral(":passw"), ui->edtPassword->text());

    if (query.exec()) {
        query.next();
        QSqlRecord rec = query.record();
        int qSize = query.value(rec.indexOf(QStringLiteral("cnt"))).toInt();
        if (qSize > 0) {
           result = true;
        }
        ++countTry;
        if (countTry == 3 && !result) {
            close();
        } else if (!result) {
            QMessageBox::warning(this, tr("Failed password entry!"),
                                 QString(tr("Incorrect login or password. \
                                            \nAttempts remaining: %1")).arg((3 - countTry)));
        }
    } else {
        QMessageBox::warning(this, tr("Error execution sql query!"),
                             query.lastError().driverText());
    }

    return result;
}
//--------------------------------------------------------------------------------------------------

void FrmLogin::animationForm()
{
    int countElements = m_listElements.size();
    this->setMinimumSize(0,0);
    for (int i = 0; i < countElements; ++i) {
        QPropertyAnimation *animation = new QPropertyAnimation(m_listElements.at(i), "geometry");
        animation->setDuration(1500);
        animation->setEasingCurve(QEasingCurve::Linear);
        animation->setEndValue(QRect(ui->btnEnter->geometry().x()
                                     + ui->btnEnter->geometry().width() / 2,
                                     ui->btnEnter->geometry().y()
                                     + ui->btnEnter->geometry().height() / 2, 0, 0));
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
//--------------------------------------------------------------------------------------------------

void FrmLogin::animationFinish()
{
    this->accept();
}

//--------------------------------------------------------------------------------------------------

void FrmLogin::frmClose()
{
    this->reject();
}
//--------------------------------------------------------------------------------------------------

void FrmLogin::setDbName(QString value)
{
    m_dbName = value;
}
//--------------------------------------------------------------------------------------------------

QString FrmLogin::dbName()
{
    return m_dbName;
}
//--------------------------------------------------------------------------------------------------

FrmLogin::~FrmLogin()
{
    m_db.close();
    delete ui;
}
//--------------------------------------------------------------------------------------------------
