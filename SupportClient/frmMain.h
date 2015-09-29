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



namespace Ui {
class FrmMain;
}

class FrmMain : public QMainWindow
{
    Q_OBJECT

public:
    explicit FrmMain(QWidget *parent = 0);
    ~FrmMain();

    void initDB(QString dbName);
    void setUserInfo(QString user, QString name1, QString name2, QString name3);

protected:
    bool event(QEvent *p_event);
    void keyPressEvent(QKeyEvent* evnt);

private:
    struct UserInfo{
        QString user;
        QString name1;
        QString name2;
        QString name3;
    };

    Ui::FrmMain *ui;
    UserInfo m_userInfo;

    /*Network*/
    QUdpSocket* m_udpSock;

    /*DB*/
    QTableWidget *m_tableWidget;
    QVBoxLayout *m_vLayout;
    QSqlQueryModel *m_model;
    QStandardItemModel *m_modelExtInfo ;

    QSqlDatabase m_db;

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
