#ifndef FRMMAIN_H
#define FRMMAIN_H

#include <QMainWindow>
#include <QUdpSocket>
#include <QTimer>
#include <QDateTime>

namespace Ui {
class frmMain;
}

class frmMain : public QMainWindow
{
    Q_OBJECT

public:
    explicit frmMain(QWidget *parent = 0);
    ~frmMain();

private:
    Ui::frmMain *ui;

    QUdpSocket* udpSock;
    QTimer* pTimer;

    void generatePhone();

protected slots:
    void slotSendDatagram();
    void slotSendDatagramAuto();
};

#endif // FRMMAIN_H
