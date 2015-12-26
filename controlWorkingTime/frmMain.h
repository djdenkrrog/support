#ifndef FRMMAIN_H
#define FRMMAIN_H

#include <QMainWindow>

#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSystemTrayIcon>
#include <QCloseEvent>

namespace Ui {
class FrmMain;
}

class FrmMain : public QMainWindow
{
    Q_OBJECT

public:
    explicit FrmMain(QWidget *parent = 0);
    ~FrmMain();

    bool initDB(QString dbName);

    void setVisible(bool visible) Q_DECL_OVERRIDE;

protected:
    bool event(QEvent * e) Q_DECL_OVERRIDE;
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

private:
    Ui::FrmMain *ui;

    void createActions();
    void createTrayIcon();

    void refreshTable();
    void insertDBegin();
    void insertDEnd();

    QString getDayOfWeekToString(QDateTime);

    QSqlDatabase m_db;
    QSqlQueryModel *m_model;
    QString m_dbName;

    QAction *minimizeAction;
    QAction *maximizeAction;
    QAction *restoreAction;
    QAction *quitAction;

    QSystemTrayIcon *m_trayIcon;
    QMenu *m_trayIconMenu;

protected slots:
    void onClickBtnComing();
    void onClickBtnLeave();
    void onCloseApp();
    void onAbout();
    void onShowCalcTime();
    void onEditCheckClick(bool);
    void onRemoveClick();
    void iconActivated(QSystemTrayIcon::ActivationReason reason);

};
#endif // FRMMAIN_H
