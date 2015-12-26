#include "frmCalcTime.h"
#include "ui_frmCalcTime.h"

#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>

FrmCalcTime::FrmCalcTime(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FrmCalcTime)
{
    ui->setupUi(this);

    ui->dtBegin->setDate(QDate::currentDate().fromString(QDate::currentDate().toString("01/MM/yyyy"), "dd/MM/yyyy"));
    ui->dtEnd->setDate(QDate::currentDate());

    connect(ui->btnCalc, SIGNAL(clicked(bool)), SLOT(onCalcClick()));
    connect(ui->btnClear, SIGNAL(clicked(bool)), SLOT(onClearClick()));

}
//----------------------------------------------------------------------------------------------------------------------

void FrmCalcTime::onCalcClick()
{
    qDebug()<<"onCalcClick";
    if (ui->dtBegin->date() > ui->dtEnd->date()) {
        QMessageBox::warning(this, tr("Warning!"), tr("Incorrectly set date"));
        return;
    }

    int cntMin;
    int cntHour;
    QSqlQuery query;
    query.prepare(QStringLiteral("select count(cnt_minutes) cnt from work_time where strftime('%Y-%m-%d', d_begin) between :dBegin and :dEnd;"));

    query.bindValue(QStringLiteral(":dBegin"), ui->dtBegin->date().toString(QStringLiteral("yyyy-MM-dd")));
    query.bindValue(QStringLiteral(":dEnd"), ui->dtEnd->date().toString(QStringLiteral("yyyy-MM-dd")));

    if (query.exec() && query.next()) {
        if (query.value(0).toInt() > 0) {
            query.clear();
            query.prepare(QStringLiteral("Select sum(cnt_min) cnt_m from (select case when \
                                         ((day_of_week = 'Sat' or  day_of_week = 'Sun' or hd.d_holiday is not null) \
                                         and ww.d_working_weekend is null) then wt.cnt_minutes * 2 else \
                                         wt.cnt_minutes - 60 end as cnt_min from work_time wt left outer \
                                         join working_weekends ww on ww.d_working_weekend = substr(wt.d_begin,1,10) \
                                         left outer join holidays hd on hd.d_holiday = substr(wt.d_begin,1,10) \
                                         where strftime('%Y-%m-%d',d_begin) between :dBegin and :dEnd)"));
            query.bindValue(QStringLiteral(":dBegin"), ui->dtBegin->date().toString(QStringLiteral("yyyy-MM-dd")));
            query.bindValue(QStringLiteral(":dEnd"), ui->dtEnd->date().toString(QStringLiteral("yyyy-MM-dd")));

            if (query.exec() && query.next()) {
                cntMin = query.value(0).toInt();
                cntHour = cntMin / 60;
                cntMin = cntMin - (cntHour * 60);
                ui->edtResult->insertPlainText(QString(tr("Total work-time with %1 to %2 : %3h., %4min.\n")).
                                            arg(ui->dtBegin->date().toString("dd/MM/yyyy")).
                                            arg(ui->dtEnd->date().toString("dd/MM/yyyy")).
                                            arg(cntHour).arg(cntMin));

                query.clear();
                query.prepare(QStringLiteral("select strftime('%d.%m.%Y %H:%M:%S', wt.d_begin) as d_begin, \
                                             strftime('%d.%m.%Y %H:%M:%S', wt.d_end) as d_end, \
                                             wt.day_of_week as day_of_week, \
                                             case when hd.d_holiday is null then 'F' else 'T' end as d_hol, \
                                             case when ww.d_working_weekend is null then 'F' else 'T' end as d_workweek, \
                                             wt.cnt_minutes as cnt_minutes, \
                                             case when ((wt.day_of_week = 'Sat' or  day_of_week = 'Sun' or hd.d_holiday \
                                             is not null) and  ww.d_working_weekend is null) then wt.cnt_minutes * 2 \
                                             else  wt.cnt_minutes - 60 end  as cnt_min from work_time wt left outer \
                                             join working_weekends ww on  ww.d_working_weekend = substr(wt.d_begin,1,10) \
                                             left outer join holidays hd on hd.d_holiday = substr(wt.d_begin,1,10) where \
                                             strftime('%Y-%m-%d',d_begin) between :dBegin and :dEnd"));
                query.bindValue(QStringLiteral(":dBegin"), ui->dtBegin->date().toString(QStringLiteral("yyyy-MM-dd")));
                query.bindValue(QStringLiteral(":dEnd"), ui->dtEnd->date().toString(QStringLiteral("yyyy-MM-dd")));
                if (query.exec() && query.next()) {
                    ui->edtResult->insertPlainText(QString(tr("Schedule:\n")));
                    ui->edtResult->insertPlainText(QString(tr("d_begin            |d_end              |dow|h|w|cnt|cnt|\n")));
                    query.first();
                    while (query.next()) {
                        ui->edtResult->insertPlainText(QString("%1|%2|%3|%4|%5|%6|%7|\n").
                                                       arg(query.value(0).toString()).
                                                       arg(query.value(1).toString()).
                                                       arg(query.value(2).toString()).
                                                       arg(query.value(3).toString()).
                                                       arg(query.value(4).toString()).
                                                       arg(query.value(5).toString()).
                                                       arg(query.value(6).toString()));
                    }
                }
            } else {
                QMessageBox::critical(this, tr("Error!"), tr("Error execution sql query!") + "\n"
                                     + query.lastError().driverText());
            }

        } else {
            QMessageBox::warning(this, tr("Warning!"), tr("For the selected period no data."));
            return;
        }

    } else {
        QMessageBox::critical(this, tr("Error!"), tr("Error execution sql query!") + "\n"
                             + query.lastError().driverText());
    }
}
//----------------------------------------------------------------------------------------------------------------------

void FrmCalcTime::onClearClick()
{
    ui->edtResult->clear();
}
//----------------------------------------------------------------------------------------------------------------------

FrmCalcTime::~FrmCalcTime()
{
    delete ui;
}
//----------------------------------------------------------------------------------------------------------------------

