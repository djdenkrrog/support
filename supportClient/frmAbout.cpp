#include "frmAbout.h"
#include "ui_frmAbout.h"
#include <QPalette>
#include <QPainter>
#include <QImage>
#include <QPixmap>

FrmAbout::FrmAbout(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FrmAbout)
{
    QString nameProg;
    QString note;

    ui->setupUi(this);

    this->setWindowFlags(Qt::Dialog);

    connect(ui->btnClose, SIGNAL(clicked(bool)), SLOT(close()));

    nameProg = tr("Programm Support v.: 1.0.0.1");
    note = tr("Program Support is provided for recording incoming calls operator." \
              "\nDemo demonstrates how to: " \
              "\n 1. Data base;" \
              "\n 2. XML documents;" \
              "\n 3. Work with network;" \
              "\n 4. Work with animation;"
              );
    ui->lblNameProg->setText(nameProg);
    ui->lblNote->setText(note);
    ui->lblImg->setScaledContents(true);
    ui->lblImg->setPixmap(QPixmap(QStringLiteral(":/images/support.png")));


}
//--------------------------------------------------------------------------------------------------

FrmAbout::~FrmAbout()
{
    delete ui;
}
//--------------------------------------------------------------------------------------------------
