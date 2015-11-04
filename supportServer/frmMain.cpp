#include "frmMain.h"
#include "ui_frmMain.h"

FrmMain::FrmMain(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FrmMain)
{
    ui->setupUi(this);

    setWindowTitle("SupportUdpServer");
    m_udpSock = new QUdpSocket(this);
    m_pTimer = new QTimer(this);
    QTime midnight(0, 0, 0);
    qsrand(midnight.secsTo(QTime::currentTime()));

    connect(ui->chbAutoSend, SIGNAL(clicked(bool)), SLOT(slotSendDatagramAuto()));
    connect(ui->btnSend, SIGNAL(clicked(bool)), SLOT(slotSendDatagram()));
    connect(m_pTimer, SIGNAL(timeout()), SLOT(slotSendDatagram()));
}
//--------------------------------------------------------------------------------------------------

void FrmMain::slotSendDatagram()
{
    qDebug() << "slotSendDatagram";
    QByteArray baDatagram;
    QDataStream out(&baDatagram, QIODevice::WriteOnly);
    if (ui->chbAutoSend->isChecked()) {
        generatePhone();
    }
    out << ui->edtPhone->text();
    m_udpSock->writeDatagram(baDatagram, QHostAddress(ui->edtIP->text()),
                             ui->edtUdpPort->text().toInt());
}
//--------------------------------------------------------------------------------------------------

void FrmMain::slotSendDatagramAuto()
{
    if (ui->chbAutoSend->isChecked()) {
        ui->btnSend->setEnabled(false);
        ui->edtPhone->setReadOnly(true);
        m_pTimer->setInterval(ui->edtInterval->text().toInt());
        m_pTimer->start();
    } else {
        ui->btnSend->setEnabled(true);
        ui->edtPhone->setReadOnly(false);
        m_pTimer->stop();
    }
}
//--------------------------------------------------------------------------------------------------

void FrmMain::generatePhone()
{
    ui->edtPhone->setText(QString("%1%2%3%4%5%6%7%8%9%10").arg(qrand()%10).arg(qrand()%10). \
                          arg(qrand()%10).arg(qrand()%10).arg(qrand()%10).arg(qrand()%10). \
                          arg(qrand()%10).arg(qrand()%10). arg(qrand()%10).arg(qrand()%10));
}
//--------------------------------------------------------------------------------------------------

FrmMain::~FrmMain()
{
    delete ui;
}
//--------------------------------------------------------------------------------------------------

