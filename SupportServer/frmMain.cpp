#include "frmMain.h"
#include "ui_frmMain.h"

FrmMain::FrmMain(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FrmMain)
{
    ui->setupUi(this);

    setWindowTitle("SupportUdpServer");
    udpSock = new QUdpSocket(this);
    pTimer = new QTimer(this);
    QTime midnight(0,0,0);
    qsrand(midnight.secsTo(QTime::currentTime()));

    connect(ui->chbAutoSend, SIGNAL(clicked(bool)), SLOT(slotSendDatagramAuto()));
    connect(ui->btnSend, SIGNAL(clicked(bool)), SLOT(slotSendDatagram()));
    connect(pTimer, SIGNAL(timeout()), SLOT(slotSendDatagram()));
}
//----------------------------------------------------------------------------------------------------------------------

void FrmMain::slotSendDatagram()
{
    qDebug() << "slotSendDatagram";
    QByteArray baDatagram;
    QDataStream out(&baDatagram, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_5);
    if (ui->chbAutoSend->isChecked()) {
        generatePhone();
    }
    out << ui->edtPhone->text();
    udpSock->writeDatagram(baDatagram, QHostAddress(ui->edtIP->text()), ui->edtUdpPort->text().toInt());
}
//----------------------------------------------------------------------------------------------------------------------

void FrmMain::slotSendDatagramAuto()
{
    if (ui->chbAutoSend->isChecked()) {
        ui->btnSend->setEnabled(false);
        ui->edtPhone->setReadOnly(true);
        pTimer->setInterval(ui->edtInterval->text().toInt());
        pTimer->start();
    } else {
        ui->btnSend->setEnabled(true);
        ui->edtPhone->setReadOnly(false);
        pTimer->stop();
    }
}
//----------------------------------------------------------------------------------------------------------------------

void FrmMain::generatePhone()
{
    ui->edtPhone->setText(QString("%1%2%3%4%5%6%7%8%9%10").arg(qrand()%10).arg(qrand()%10).arg(qrand()%10). \
                          arg(qrand()%10).arg(qrand()%10).arg(qrand()%10).arg(qrand()%10).arg(qrand()%10). \
                          arg(qrand()%10).arg(qrand()%10));
}
//----------------------------------------------------------------------------------------------------------------------

FrmMain::~FrmMain()
{
    delete ui;
}
//----------------------------------------------------------------------------------------------------------------------

