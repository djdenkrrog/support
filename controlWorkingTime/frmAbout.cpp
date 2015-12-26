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

    nameProg = tr("Control working time");
    note = tr("Program \"Control working time\" for recording and counting of working time." \
              "\nDemonstrates how to: " \
              "\n 1. Data base;"
              );
    ui->lblNameProg->setText(nameProg);
    ui->lblNote->setText(note);
    ui->lblImg->setScaledContents(true);
    ui->lblImg->setPixmap(QPixmap(QStringLiteral(":/images/icon.png")));


}
//--------------------------------------------------------------------------------------------------

FrmAbout::~FrmAbout()
{
    delete ui;
}
//--------------------------------------------------------------------------------------------------
