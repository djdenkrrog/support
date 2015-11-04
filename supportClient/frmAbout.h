#ifndef FRMABOUT_H
#define FRMABOUT_H

#include <QMainWindow>
#include <QString>

namespace Ui {
class FrmAbout;
}

class FrmAbout : public QMainWindow
{
    Q_OBJECT

public:
    explicit FrmAbout(QWidget *parent = 0);
    ~FrmAbout();

private:
    Ui::FrmAbout *ui;
};

#endif // FRMABOUT_H
