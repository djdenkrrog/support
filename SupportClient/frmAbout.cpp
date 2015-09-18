#include "frmAbout.h"
#include "ui_frmAbout.h"
#include <QPalette>
#include <QPainter>
#include <QImage>
#include <QPixmap>

frmAbout::frmAbout(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::frmAbout)
{
    QString nameProg;
    QString note;

    ui->setupUi(this);

    this->setWindowFlags(Qt::Dialog);

    connect(ui->btnClose, SIGNAL(clicked(bool)), SLOT(close()));

    nameProg = tr("Программа Support v.: 1.0.0.1");
    note = tr("Программа Support предназначена для записи операторм входящих звонков." \
              "\nДемо версия демонстрирует работу с: " \
              "\n 1. Базами данных;" \
              "\n 2. XML документами;" \
              "\n 3. Работа с сетью;" \
              "\n 4. Работа с анмацией;"
              );
    ui->lblNameProg->setText(nameProg);
    ui->lblNote->setText(note);
    ui->lblImg->setScaledContents(true);
    ui->lblImg->setPixmap(QPixmap(":/images/support.png"));


}
//----------------------------------------------------------------------------------------------------------------------

frmAbout::~frmAbout()
{
    delete ui;
}
//----------------------------------------------------------------------------------------------------------------------
