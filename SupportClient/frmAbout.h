#ifndef FRMABOUT_H
#define FRMABOUT_H

#include <QMainWindow>
#include <QString>

namespace Ui {
class frmAbout;
}

class frmAbout : public QMainWindow
{
    Q_OBJECT

public:
    explicit frmAbout(QWidget *parent = 0);
    ~frmAbout();

private:
    Ui::frmAbout *ui;
};

#endif // FRMABOUT_H
