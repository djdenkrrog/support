#ifndef FRMMAIN_H
#define FRMMAIN_H

#include <QMainWindow>
#include <QUdpSocket>
#include <QTimer>
#include <QDateTime>

namespace Ui {
class FrmMain;
}

class FrmMain : public QMainWindow
{
    Q_OBJECT

public:
    explicit FrmMain(QWidget *parent = 0);
    ~FrmMain();

private:
    Ui::FrmMain *ui;

    QUdpSocket* udpSock;
    QTimer* pTimer;

    void generatePhone();

protected slots:
    void slotSendDatagram();
    void slotSendDatagramAuto();
};

#endif // FRMMAIN_H
