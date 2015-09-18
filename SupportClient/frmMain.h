#ifndef frmMain_H
#define frmMain_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QTableWidget>
#include <QDebug.h>
#include <QBoxLayout.h>
#include <QSqlQueryModel>
#include <QStandardItemModel>
#include <QDateTime>
#include <QtXml>
#include <QUdpSocket>
#include <QKeyEvent>

//Флаги состояния
struct TuserInfo{
    QString user;
    QString name1;
    QString name2;
    QString name3;
};

namespace Ui {
class frmMain;
}

class frmMain : public QMainWindow
{
    Q_OBJECT

public:
    explicit frmMain(QWidget *parent = 0);
    ~frmMain();

    void initDB(QString dbName);
    TuserInfo userInfo;

protected:
    bool event(QEvent *p_event);
    void keyPressEvent(QKeyEvent* evnt);

private:
    Ui::frmMain *ui;

    /*Network*/
    QUdpSocket* udpSock;

    /*DB*/
    QTableWidget *tableWidget;
    QVBoxLayout *vlayout;
    QSqlQueryModel *model;
    QStandardItemModel *modelExtInfo ;

    QSqlDatabase db;

    void refreshTable();

    /*XML*/
    QDomDocument xmlCreate();

    QDomElement xmlCreateItemCall(
            QDomDocument& domDoc,
            const QString& strUser,
            const QString& strName1,
            const QString& strName2,
            const QString& strName3,
            const QString& strPhone,
            const QString& strQuestion,
            const QString& strAnswer,
            const QString& strCallDBegin,
            const QString& strCallDEnd
            );

    QDomElement xmlMakeElement(
            QDomDocument& domDoc,
            const QString& strName,
            const QString& strAttr = QString::null,
            const QString& strText = QString::null
            );

    void eachNode(const QDomNode& node);
    void addItemToDB(const QDomNode& node);

protected slots:
    void appClose();
    void callAdd();
    void callSave();
    void moveItem();
    void loadFromXML();
    void saveToXML();
    void aboutShow();
    void processDatagrams();
    void changedInsertCall();
    void selectionChanged();
};

#endif // frmMain_H
