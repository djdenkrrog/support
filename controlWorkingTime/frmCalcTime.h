#ifndef FRMCALCTIME_H
#define FRMCALCTIME_H

#include <QWidget>

namespace Ui {
class FrmCalcTime;
}

class FrmCalcTime : public QWidget
{
    Q_OBJECT

public:
    explicit FrmCalcTime(QWidget *parent = 0);
    ~FrmCalcTime();

private:
    Ui::FrmCalcTime *ui;

protected slots:
    void onCalcClick();
    void onClearClick();
};

#endif // FRMCALCTIME_H
