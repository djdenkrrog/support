#include "frmMain.h"
#include "ui_frmMain.h"

#include "frmCalcTime.h"
#include "frmAbout.h"

#include <QDebug>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlQueryModel>
#include <QSqlRecord>
#include <QDateTime>
#include <QFileDialog>
#include <QProcess>

#define version "1.0.0.1"

FrmMain::FrmMain(QWidget *parent) : QMainWindow(parent), ui(new Ui::FrmMain)
{
    ui->setupUi(this);
    setWindowTitle(tr("Control working time (v.:") + version + ")");

    ui->dtEdit->setDateTime(QDateTime::currentDateTime());

    ui->btnExit->setIcon(QIcon(QStringLiteral(":/images/exit.png")));
    ui->actionExit->setIcon(QIcon(QStringLiteral(":/images/exit.png")));
    ui->btnComing->setIcon(QIcon(QStringLiteral(":/images/coming64.png")));
    ui->btnLeave->setIcon(QIcon(QStringLiteral(":/images/leave64.png")));
    ui->actionAbout->setIcon(QIcon(QStringLiteral(":/images/about.png")));

    m_dbName = QString::null;

    m_model = new QSqlQueryModel();
    ui->tblTimeWork->setModel(m_model);

    connect(ui->btnComing, SIGNAL(clicked(bool)), SLOT(onClickBtnComing()));
    connect(ui->btnLeave, SIGNAL(clicked(bool)), SLOT(onClickBtnLeave()));
    connect(ui->btnExit, SIGNAL(clicked(bool)), SLOT(onCloseApp()));
    connect(ui->actionExit, SIGNAL(triggered(bool)), SLOT(onCloseApp()));
    connect(ui->chbIsEdit, SIGNAL(toggled(bool)), SLOT(onEditCheckClick(bool)));
    connect(ui->btnRemove, SIGNAL(clicked(bool)), SLOT(onRemoveClick()));
    connect(ui->actionAbout, SIGNAL(triggered(bool)), SLOT(onAbout()));


    connect(ui->actionCountingTime, SIGNAL(triggered(bool)), SLOT(onShowCalcTime()));

    createActions();
    createTrayIcon();
    connect(m_trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
    m_trayIcon->setIcon(QIcon(":images/icon.png"));
    m_trayIcon->setToolTip(tr("Control working time"));
    m_trayIcon->show();

}
//----------------------------------------------------------------------------------------------------------------------

bool FrmMain::initDB(QString dbName)
{
    m_dbName = dbName;

    m_db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"));
    m_db.setDatabaseName(m_dbName);
    if (!m_db.open()) {
        QMessageBox::warning(this, tr("Error opening DB!"), m_db.lastError().databaseText());
        return false;
    }

    refreshTable();
    return true;
}
//----------------------------------------------------------------------------------------------------------------------

void FrmMain::refreshTable()
{
    QSqlQuery query(QStringLiteral("select day_of_week, d_begin, d_end, cnt_minutes, rowid from work_time \
                                    order by d_begin desc;"));

    if (query.exec()) {
        m_model->setQuery(query);
        m_model->setHeaderData(0, Qt::Horizontal, tr("Day"));
        m_model->setHeaderData(1, Qt::Horizontal, tr("Begin"));
        m_model->setHeaderData(2, Qt::Horizontal, tr("End"));
        m_model->setHeaderData(3, Qt::Horizontal, tr("Count min."));

        if (m_model->lastError().isValid()) {
            qDebug() << m_model->lastError();
            QMessageBox::warning(this, tr("Error data model!"), m_model->lastError().driverText());
        }

        ui->tblTimeWork->resizeRowsToContents();
        ui->tblTimeWork->resizeColumnsToContents();
        ui->tblTimeWork->hideColumn(4);
        ui->tblTimeWork->selectRow(0);
    } else {
        qDebug() << "query.lastQuery " << query.lastQuery();
        QMessageBox::warning(this, tr("Error execution sql query!"), query.lastError().driverText());
    }

}
//----------------------------------------------------------------------------------------------------------------------

void FrmMain::insertDBegin()
{
    QSqlQuery query;
    query.prepare(QStringLiteral("INSERT INTO work_time(d_begin, day_of_week) VALUES(:dBegin, :dayOfWeek);"));

    if (ui->chbIsEdit->isChecked()) {
        query.bindValue(QStringLiteral(":dBegin"), ui->dtEdit->dateTime().toString(QStringLiteral("yyyy-MM-dd hh:mm:ss")));
        query.bindValue(QStringLiteral(":dayOfWeek"), getDayOfWeekToString(ui->dtEdit->dateTime()));
    } else {
        query.bindValue(QStringLiteral(":dBegin"), QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd hh:mm:ss")));
        query.bindValue(QStringLiteral(":dayOfWeek"), getDayOfWeekToString(QDateTime::currentDateTime()));
    }

    if (query.exec()) {
        m_trayIcon->showMessage(windowTitle(), "Added date time coming", QSystemTrayIcon::Information, 2000);
        refreshTable();
    } else {
         QMessageBox::critical(this, tr("Error!"), tr("Error execution sql query!") + "\n"
                              + query.lastError().driverText());
    }
}
//----------------------------------------------------------------------------------------------------------------------

void FrmMain::insertDEnd()
{
    QSqlQuery query;
    if (ui->chbIsEdit->isChecked()) {
        QItemSelectionModel *select = ui->tblTimeWork->selectionModel();
        if (select->hasSelection()) {
            int rowIdx = select->currentIndex().row();
            int rowId = m_model->record(rowIdx).value(QStringLiteral("rowid")).toInt();
            QDateTime dBegin =QDateTime::fromString(m_model->record(rowIdx).value(QStringLiteral("d_begin")).toString(),
                                                    "yyyy-MM-dd hh:mm:ss");
            QDateTime dEnd = ui->dtEdit->dateTime();
            int cntMin = dBegin.secsTo(dEnd) / 60;
            query.prepare(QStringLiteral("update work_time set d_end=:dEnd, cnt_minutes=:cntMin where rowid=:id;"));
            query.bindValue(QStringLiteral(":dEnd"), dEnd.toString(QStringLiteral("yyyy-MM-dd hh:mm:ss")));
            query.bindValue(QStringLiteral(":cntMin"), cntMin);
            query.bindValue(QStringLiteral(":id"), rowId);
            if (!query.exec()) {
                QMessageBox::critical(this, tr("Error!"), tr("Error execution sql query!") + "\n"
                                     + query.lastError().driverText());
            }
            refreshTable();
        } else {
            QMessageBox::warning(this, tr("Warning!"), tr("Not selected row!"));
        }
    } else {
        query.prepare(QStringLiteral("select count(*)  from work_time where substr(d_begin,1,10)=:dBegin;"));
        query.bindValue(QStringLiteral(":dBegin"), QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd")));

        if (query.exec() && query.next()) {
            if (query.value(0).toInt() > 1) {
                QMessageBox::critical(this, tr("Error!"), tr("In the database of today more than one record!\nCannot save! "));
                return;
            } else if (query.value(0).toInt() < 1) {
                QMessageBox::critical(this, tr("Error!"), tr("The database does not have a record today!\nCannot save! "));
                return;
            }

            query.clear();
            query.prepare(QStringLiteral("select d_begin from work_time where substr(d_begin,1,10)=:dBegin;"));
            query.bindValue(QStringLiteral(":dBegin"), QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd")));

            if (query.exec() && query.next()) {
                QString dStr = query.value(0).toString();
                QDateTime dBegin = QDateTime::fromString(dStr, "yyyy-MM-dd hh:mm:ss");
                QDateTime dEnd = QDateTime::currentDateTime();
                int cntMin = dBegin.secsTo(dEnd) / 60;;

                query.clear();
                query.prepare(QStringLiteral("update work_time set d_end=:dEnd, cnt_minutes=:cntMin where substr(d_begin,1,10)=:dBegin;"));
                query.bindValue(QStringLiteral(":dEnd"), dEnd.toString(QStringLiteral("yyyy-MM-dd hh:mm:ss")));
                query.bindValue(QStringLiteral(":cntMin"), cntMin);
                query.bindValue(QStringLiteral(":dBegin"), QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd")));

                if (!query.exec()) {
                    QMessageBox::critical(this, tr("Error!"), tr("Error execution sql query!") + "\n"
                                         + query.lastError().driverText());
                }
                m_trayIcon->showMessage(windowTitle(), "Added date time leave", QSystemTrayIcon::Information, 2000);
                refreshTable();
            }
        } else {
             QMessageBox::critical(this, tr("Error!"), tr("Error execution sql query!") + "\n"
                              + query.lastError().driverText());
        }
    }
}
//----------------------------------------------------------------------------------------------------------------------

QString FrmMain::getDayOfWeekToString(QDateTime dt)
{
    int dayOfWeek;
    QString strDay;

    dayOfWeek = dt.date().dayOfWeek();

    switch (dayOfWeek) {
        case 1 : strDay = "Mon";
            break;
        case 2 : strDay = "Tue";
            break;
        case 3 : strDay = "Wed";
            break;
        case 4 : strDay = "Thu";
            break;
        case 5 : strDay = "Fri";
            break;
        case 6 : strDay = "Sat";
            break;
        case 7 : strDay = "Sun";
            break;
    }

    return strDay;
}
//----------------------------------------------------------------------------------------------------------------------

void FrmMain::onClickBtnComing()
{
    qDebug() << "onClickBtnComing";
    insertDBegin();
}
//----------------------------------------------------------------------------------------------------------------------

void FrmMain::onClickBtnLeave()
{
    qDebug() << "onClickBtnLeave";
    insertDEnd();
}
//----------------------------------------------------------------------------------------------------------------------

void FrmMain::onCloseApp()
{
    qDebug() << "closeApp";
    QString closeScript = QApplication::applicationDirPath() + "/scripts/close.sh";
    if (QFileInfo::exists(closeScript)) {
        QProcess prc;
        prc.start(closeScript);
        prc.waitForFinished(-1);
        qDebug() << "closeScript = " << closeScript;
    }
    qApp->quit();
}
//----------------------------------------------------------------------------------------------------------------------

void FrmMain::onAbout()
{
    FrmAbout *fAbout = new FrmAbout();
    fAbout->show();
}
//----------------------------------------------------------------------------------------------------------------------

void FrmMain::onShowCalcTime()
{
    qDebug()<<"onShowCalcTime";

    FrmCalcTime *frmCntTime = new FrmCalcTime;
    frmCntTime->move(this->geometry().center() - frmCntTime->rect().center());
    frmCntTime->show();
}
//----------------------------------------------------------------------------------------------------------------------

void FrmMain::onEditCheckClick(bool check)
{
    ui->btnRemove->setEnabled(check);
    ui->dtEdit->setEnabled(check);
    m_trayIcon->showMessage(windowTitle(), "Enable edit mode.", QSystemTrayIcon::Information, 2000);
    qDebug()<<"Check"<<check;

}
//---------------------------------------------------------------------------------------------------------------------

void FrmMain::onRemoveClick()
{
    QItemSelectionModel *select = ui->tblTimeWork->selectionModel();
    if (select->hasSelection()) {
        int rowIdx = select->currentIndex().row();
        int rowId = m_model->record(rowIdx).value(QStringLiteral("rowid")).toInt();

        QSqlQuery query;
        query.prepare(QStringLiteral("delete from work_time where rowid=:id"));
        query.bindValue(QStringLiteral(":id"), rowId);
        if (!query.exec()) {
            QMessageBox::critical(this, tr("Error!"), tr("Error execution sql query!") + "\n"
                                 + query.lastError().driverText());
        }
        refreshTable();
    } else {
        QMessageBox::warning(this, tr("Warning!"), tr("Not selected row!"));
    }
}
//----------------------------------------------------------------------------------------------------------------------


void FrmMain::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    qDebug() << "iconActivated";
    switch (reason) {
        case QSystemTrayIcon::Trigger:
        case QSystemTrayIcon::DoubleClick:
            qDebug() << "DoubleClick";
            this->showNormal();
            break;
        default:
            break;
    }
}
//----------------------------------------------------------------------------------------------------------------------

void FrmMain::setVisible(bool visible)
{
    qDebug() << "setVisible=" << visible;
    QMainWindow::setVisible(visible);
}
//----------------------------------------------------------------------------------------------------------------------

bool FrmMain::event(QEvent *event)
{
    if (event->type() == QEvent::WindowStateChange && isMinimized()) {
        qDebug() << "event isMinimized=" << event;
        if (m_trayIcon->isVisible()) {
            hide();
            event->ignore();
        }
    }
    return QMainWindow::event(event);
}
//----------------------------------------------------------------------------------------------------------------------

void FrmMain::closeEvent(QCloseEvent *event)
{
    if (m_trayIcon->isVisible()) {
        hide();
        event->ignore();
    }

}
//----------------------------------------------------------------------------------------------------------------------

void FrmMain::createActions()
{
//    minimizeAction = new QAction(tr("Mi&nimize"), this);
//    connect(minimizeAction, SIGNAL(triggered()), this, SLOT(hide()));

//    maximizeAction = new QAction(tr("Ma&ximize"), this);
//    connect(maximizeAction, SIGNAL(triggered()), this, SLOT(showMaximized()));

    restoreAction = new QAction(tr("&Restore"), this);
    restoreAction->setIcon(QIcon(QStringLiteral(":/images/restore64.png")));
    connect(restoreAction, SIGNAL(triggered()), this, SLOT(showNormal()));

    quitAction = new QAction(tr("&Quit"), this);
    quitAction->setIcon(QIcon(QStringLiteral(":/images/exit.png")));
    connect(quitAction, SIGNAL(triggered()), SLOT(onCloseApp()));

}
//----------------------------------------------------------------------------------------------------------------------

void FrmMain::createTrayIcon()
{
    m_trayIconMenu = new QMenu(this);
    //trayIconMenu->addAction(minimizeAction);
    //trayIconMenu->addAction(maximizeAction);
    m_trayIconMenu->addAction(restoreAction);
    m_trayIconMenu->addSeparator();
    m_trayIconMenu->addAction(quitAction);

    m_trayIcon = new QSystemTrayIcon(this);
    m_trayIcon->setContextMenu(m_trayIconMenu);

}
//----------------------------------------------------------------------------------------------------------------------

FrmMain::~FrmMain()
{
    delete ui;
}
