#include "frmMain.h"
#include "ui_frmMain.h"
#include "frmAbout.h"
#include "frmLogin.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QDateTime>

FrmMain::FrmMain(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FrmMain)
{
    ui->setupUi(this);

    this->setWindowOpacity(0);

    m_model = new QSqlQueryModel();
    ui->tblGridHistory->setModel(m_model);

    ui->btnCallAdd->setIcon(QIcon(QStringLiteral(":/images/arrowright.png")));
    ui->btnClose->setIcon(QIcon(QStringLiteral(":/images/exit.png")));
    ui->actionClose->setIcon(QIcon(QStringLiteral(":/images/exit.png")));
    ui->btnCallSave->setIcon(QIcon(QStringLiteral(":/images/add.png")));
    ui->actionAbout->setIcon(QIcon(QStringLiteral(":/images/about.png")));
    ui->actionSaveToXML->setIcon(QIcon(QStringLiteral(":/images/xml_import.png")));
    ui->actionLoadFromXML->setIcon(QIcon(QStringLiteral(":/images/xml_export.png")));
    ui->btnMoveItem->setIcon(QIcon(QStringLiteral(":/images/arrowright.png")));


    connect(ui->edtInsertCallPhone, SIGNAL(textChanged(QString)), SLOT(changedInsertCall()));
    connect(ui->actionClose, SIGNAL(triggered(bool)), SLOT(appClose()));
    connect(ui->btnClose, SIGNAL(clicked(bool)), SLOT(appClose()));
    connect(ui->btnCallSave, SIGNAL(clicked(bool)), SLOT(callSave()));
    connect(ui->btnCallAdd, SIGNAL(clicked(bool)), SLOT(callAdd()));
    connect(ui->btnMoveItem, SIGNAL(clicked(bool)), SLOT(moveItem()));
    connect(ui->actionSaveToXML, SIGNAL(triggered(bool)), SLOT(saveToXML()));
    connect(ui->actionLoadFromXML, SIGNAL(triggered(bool)), SLOT(loadFromXML()));
    connect(ui->actionAbout, SIGNAL(triggered(bool)), SLOT(aboutShow()));
    connect(ui->tblGridHistory, SIGNAL(doubleClicked(QModelIndex)), SLOT(moveItem()));
    connect(ui->tblGridHistory->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)), SLOT(selectionChanged()));

    m_udpSock = new QUdpSocket(this);
    m_udpSock->bind(8091);
    connect(m_udpSock, SIGNAL(readyRead()), SLOT(processDatagrams()));

    m_modelExtInfo = new QStandardItemModel;
}
//--------------------------------------------------------------------------------------------------

void FrmMain::initDB(QString dbName)
{
    m_db = QSqlDatabase::database();
    m_db.setDatabaseName(dbName);
    if (!m_db.open()) {
         QMessageBox::warning(this, tr("Error opening DB!"), m_db.lastError().driverText());
     } else {
         refreshTable();
    }
}
//--------------------------------------------------------------------------------------------------

void FrmMain::setUserInfo(QString user, QString name1, QString name2, QString name3)
{
    m_userInfo.user = user;
    m_userInfo.name1 = name1;
    m_userInfo.name2 = name2;
    m_userInfo.name3 = name3;

}
//--------------------------------------------------------------------------------------------------

void FrmMain::refreshTable()
{
    QSqlQuery query;

    if (!ui->edtInsertCallPhone->text().isEmpty()) {
        query.prepare(QStringLiteral("select name, phone, call_d_begin, question, answer, user_r, \
                                     name1, name2, name3 from v_calls where user_r=:user \
                                     and phone like :phone;"));
        query.bindValue(QStringLiteral(":user"), m_userInfo.user);
        query.bindValue(QStringLiteral(":phone"), ui->edtInsertCallPhone->text() + '%');
    } else {
        query.prepare(QStringLiteral("select name, phone, call_d_begin, question, answer, user_r, \
                                     name1, name2, name3 from v_calls where user_r=:user;"));
        query.bindValue(QStringLiteral(":user"), m_userInfo.user);
    }

    if (query.exec()) {
        m_model->setQuery(query);
        m_model->setHeaderData(0, Qt::Horizontal, tr("Full name"));
        m_model->setHeaderData(1, Qt::Horizontal, tr("Phone"));
        m_model->setHeaderData(2, Qt::Horizontal, tr("Date"));

        if (m_model->lastError().isValid()) {
            qDebug() << m_model->lastError();
            QMessageBox::warning(this, tr("Error data model!"), m_model->lastError().driverText());
        }

        ui->tblGridHistory->resizeRowsToContents();        
        ui->tblGridHistory->resizeColumnsToContents();
        ui->tblGridHistory->hideColumn(3);
        ui->tblGridHistory->hideColumn(4);
        ui->tblGridHistory->hideColumn(5);
        ui->tblGridHistory->hideColumn(6);
        ui->tblGridHistory->hideColumn(7);
        ui->tblGridHistory->hideColumn(8);

    } else {
        qDebug() << "query.lastQuery " << query.lastQuery();
        QMessageBox::warning(this, tr("Error execution sql query!"), query.lastError().driverText());
    }

}
//--------------------------------------------------------------------------------------------------

void FrmMain::callAdd()
{
    if (ui->edtInsertCallPhone->text().isEmpty()) {
        QMessageBox::warning(this, tr("Error!"), tr("No phone number of incoming call."));
    } else {
        ui->edtPhone->setText(ui->edtInsertCallPhone->text());
        ui->edtDBegin->setText(QDateTime::currentDateTime().toString(
                                   QStringLiteral("dd.MM.yyyy HH:mm:ss")));
        ui->edtInsertCallPhone->clear();
        ui->btnCallAdd->setEnabled(false);
        ui->btnCallSave->setEnabled(true);
    }

}
//--------------------------------------------------------------------------------------------------

void FrmMain::callSave()
{
    if (ui->edtPhone->text().isEmpty()) {
        QMessageBox::warning(this, tr("Error!"),
                             tr("No phone number of incoming call. Saving is not possible"));
        return;
    }
    QSqlQuery query;
    query.prepare(QStringLiteral("INSERT INTO calls VALUES (:user, :name1 , :name2, :name3, \
                                  :phone, :question, :answer, :call_d_begin, :call_d_end);"));
    query.bindValue(QStringLiteral(":user"), m_userInfo.user);
    query.bindValue(QStringLiteral(":name1"), ui->edtName1->text());
    query.bindValue(QStringLiteral(":name2"), ui->edtName2->text());
    query.bindValue(QStringLiteral(":name3"), ui->edtName3->text());
    query.bindValue(QStringLiteral(":phone"), ui->edtPhone->text());
    query.bindValue(QStringLiteral(":question"), ui->tedtQuestion->toPlainText());
    query.bindValue(QStringLiteral(":answer"), ui->tedtAnswer->toPlainText());
    query.bindValue(QStringLiteral(":call_d_begin"), ui->edtDBegin->text());
    query.bindValue(QStringLiteral(":call_d_end"),
                    QDateTime::currentDateTime().toString(QStringLiteral("dd.MM.yyyy HH:mm:ss")));

    if (query.exec()) {
        ui->edtPhone->clear();
        ui->edtDBegin->clear();
        ui->edtName1->clear();
        ui->edtName2->clear();
        ui->edtName3->clear();
        ui->tedtQuestion->clear();
        ui->tedtAnswer->clear();

        ui->btnCallAdd->setEnabled(true);
        ui->btnCallSave->setEnabled(false);

        refreshTable();
    } else {
         QMessageBox::warning(this, tr("Error!"), tr("Error execution sql query!") + "\n"
                              + query.lastError().driverText());
    }
}
//--------------------------------------------------------------------------------------------------

void FrmMain::moveItem()
{
    QItemSelectionModel *select = ui->tblGridHistory->selectionModel();
    if (select->hasSelection()) {
        int rowIdx = select->currentIndex().row();

        ui->edtName1->setText(m_model->record(rowIdx).value(QStringLiteral("name1")).toString());
        ui->edtName2->setText(m_model->record(rowIdx).value(QStringLiteral("name2")).toString());
        ui->edtName3->setText(m_model->record(rowIdx).value(QStringLiteral("name3")).toString());
    }
}
//--------------------------------------------------------------------------------------------------


void FrmMain::selectionChanged()
{
    QItemSelectionModel *select = ui->tblGridHistory->selectionModel();
    if (select->hasSelection()) {
        int rowIdx = select->currentIndex().row();

        QStandardItem *itemQuestion;
        QStandardItem *itemAnswer;
        QStringList horizontalHeader;

        m_modelExtInfo->clear();

        itemQuestion = new QStandardItem(QString(m_model->record(rowIdx).
                                                 value(QStringLiteral("question")).toString()));
        itemAnswer = new QStandardItem(QString(m_model->record(rowIdx).
                                               value(QStringLiteral("answer")).toString()));
        m_modelExtInfo->setItem(0, 0, itemQuestion);
        m_modelExtInfo->setItem(0, 1, itemAnswer);

        horizontalHeader.append(tr("Question"));
        horizontalHeader.append(tr("Answer"));
        m_modelExtInfo->setHorizontalHeaderLabels(horizontalHeader);

        ui->tblExtendedInfo->setModel(m_modelExtInfo);
        ui->tblExtendedInfo->setColumnWidth(0, 190);
        ui->tblExtendedInfo->setColumnWidth(1, 190);
    }
}
//--------------------------------------------------------------------------------------------------

void FrmMain::changedInsertCall()
{
    refreshTable();
}
//--------------------------------------------------------------------------------------------------

void FrmMain::loadFromXML()
{
    QString fileName;

    fileName = QFileDialog::getOpenFileName(this, tr("Open file"),
                                            QDir::currentPath(),tr("XML (*.xml)"));
    if (!fileName.isEmpty()) {
        QDomDocument domDoc;
        QFile file(fileName);

        if(file.open(QIODevice::ReadOnly)) {
            if(domDoc.setContent(&file)) {
                QDomElement domElement = domDoc.documentElement();
                eachNode(domElement);
            }
            file.close();
            QMessageBox::information(this, tr("Loading an XML!"), tr("Loading an XML completed."));
        }
    }
}
//--------------------------------------------------------------------------------------------------

void FrmMain::eachNode(const QDomNode& node)
{
   QDomNode domNode = node.firstChild();
   while(!domNode.isNull()) {
       if(domNode.isElement()) {
          QDomElement domElement = domNode.toElement();
          if(!domElement.isNull()) {
              if(domElement.tagName() == QLatin1String("callInfo")) {
                  addItemToDB(domElement);
              }
          }
       }
       eachNode(domNode);
       domNode = domNode.nextSibling();
    }
}
//--------------------------------------------------------------------------------------------------

void FrmMain::addItemToDB(const QDomNode& node)
{
    QString fields;
    QString values;
    QString sqlTxt;
    QDomNode domNode = node.firstChild();
    QSqlQuery query;

    while(!domNode.isNull()) {
        if(domNode.isElement()) {
           QDomElement domElement = domNode.toElement();
           if(!domElement.isNull()) {
                if (!fields.isEmpty()) {
                    fields += QStringLiteral(",") + domElement.tagName();
                    values += QStringLiteral(",'") + domElement.text() + QStringLiteral("'");
                } else {
                    fields = domElement.tagName();
                    values = QStringLiteral("'") + domElement.text() + QStringLiteral("'");
                }
           }
        }
        eachNode(domNode);
        domNode = domNode.nextSibling();
    }

    sqlTxt = QStringLiteral("insert into calls (") + fields + QStringLiteral(") values (")
            + values + QStringLiteral(");");
    query.prepare(sqlTxt);
    if (!query.exec()) {
        QMessageBox::warning(this, tr("Error!"), tr("SQL: ") + sqlTxt + tr("\nDB_Error: ")
                             + m_db.lastError().driverText());
    }
    refreshTable();
}
//--------------------------------------------------------------------------------------------------

void FrmMain::saveToXML()
{
    QString fileName;

    fileName = QFileDialog::getSaveFileName(this, tr("Save file"), QDir::currentPath(),
                                            tr("XML (*.xml)"));
    if (!fileName.isEmpty()) {
        QDomDocument resultXML = xmlCreate();

        QFile file(fileName);
        if(file.open(QIODevice::WriteOnly)) {
            QTextStream(&file) << resultXML.toString();
            file.close();
            QMessageBox::information(this, tr("Unload to XML!"), tr("Unload to XML completed."));
        }
    }
}
//--------------------------------------------------------------------------------------------------

QDomDocument FrmMain::xmlCreate()
{
    QDomDocument doc(QStringLiteral("support"));
    QDomElement  domElement = doc.createElement(QStringLiteral("calls"));
    QSqlQuery query;

    doc.appendChild(domElement);

    if (query.exec(QStringLiteral("select * from calls;"))) {
        while (query.next()) {
            QDomElement itemCall = xmlCreateItemCall(
                        doc,
                        query.value(QStringLiteral("user_r")).toString(),
                        query.value(QStringLiteral("name1")).toString(),
                        query.value(QStringLiteral("name2")).toString(),
                        query.value(QStringLiteral("name3")).toString(),
                        query.value(QStringLiteral("phone")).toString(),
                        query.value(QStringLiteral("question")).toString(),
                        query.value(QStringLiteral("answer")).toString(),
                        query.value(QStringLiteral("call_d_begin")).toString(),
                        query.value(QStringLiteral("call_d_end")).toString()
                        );
            domElement.appendChild(itemCall);
        }
    }

    return doc;
}
//--------------------------------------------------------------------------------------------------

QDomElement FrmMain::xmlCreateItemCall(
        QDomDocument& domDoc,
        const QString& strUser,
        const QString& strName1,
        const QString& strName2,
        const QString& strName3,
        const QString& strPhone,
        const QString& strQuestion,
        const QString& strAnswer,
        const QString& strCallDBegin,
        const QString& strCallDEnd
        )
{
    static int nNumber = 1;

    QDomElement domElement = xmlMakeElement(domDoc, "callInfo", QString().number(nNumber));

    domElement.appendChild(xmlMakeElement(domDoc, QStringLiteral("user_r"), QString(), strUser));
    domElement.appendChild(xmlMakeElement(domDoc, QStringLiteral("name1"), QString(), strName1));
    domElement.appendChild(xmlMakeElement(domDoc, QStringLiteral("name2"), QString(), strName2));
    domElement.appendChild(xmlMakeElement(domDoc, QStringLiteral("name3"), QString(), strName3));
    domElement.appendChild(xmlMakeElement(domDoc, QStringLiteral("phone"), QString(), strPhone));
    domElement.appendChild(xmlMakeElement(domDoc, QStringLiteral("question"), QString(),
                                          strQuestion));
    domElement.appendChild(xmlMakeElement(domDoc, QStringLiteral("answer"), QString(), strAnswer));
    domElement.appendChild(xmlMakeElement(domDoc, QStringLiteral("call_d_begin"), QString(),
                                          strCallDBegin));
    domElement.appendChild(xmlMakeElement(domDoc, QStringLiteral("call_d_end"), QString(),
                                          strCallDEnd));

    nNumber++;

    return domElement;
}
//--------------------------------------------------------------------------------------------------

QDomElement FrmMain::xmlMakeElement(
         QDomDocument& domDoc,
         const QString& strName,
         const QString& strAttr,
         const QString& strText
         )
{
    QDomElement domElement = domDoc.createElement(strName);

    if (!strAttr.isEmpty()) {
        QDomAttr domAttr = domDoc.createAttribute(QStringLiteral("number"));
        domAttr.setValue(strAttr);
        domElement.setAttributeNode(domAttr);
    }

    if (!strText.isEmpty()) {
        QDomText domText = domDoc.createTextNode(strText);
        domElement.appendChild(domText);
    }
    return domElement;
}
//--------------------------------------------------------------------------------------------------

void FrmMain::processDatagrams()
{
    QByteArray baDatagram;

    do {
        baDatagram.resize(m_udpSock->pendingDatagramSize());
        m_udpSock->readDatagram(baDatagram.data(), baDatagram.size());
    } while(m_udpSock->hasPendingDatagrams());

    QString callPhone;
    QDataStream dataStream(&baDatagram, QIODevice::ReadOnly);

    dataStream.setVersion(QDataStream::Qt_5_0);
    dataStream >> callPhone;

    ui->edtInsertCallPhone->setText(callPhone);
}
//--------------------------------------------------------------------------------------------------

void FrmMain::aboutShow()
{
    FrmAbout *fAbout;

    fAbout = new FrmAbout();
    fAbout->show();
}
//--------------------------------------------------------------------------------------------------

void FrmMain::keyPressEvent(QKeyEvent* evnt)
{
    if (evnt->modifiers() == Qt::CTRL) {
        switch (evnt->key()) {
            case Qt::Key_S : callSave();
                break;
            case Qt::Key_D : callAdd();
                break;
            case Qt::Key_W : moveItem();
                break;
        }
    }
}
//--------------------------------------------------------------------------------------------------

bool FrmMain::event(QEvent *p_event)
{
    if( p_event->type() == QEvent::ShowToParent) {
           setWindowTitle(windowTitle() + QString(QStringLiteral(" - %1 %2 %3"))
                          .arg(m_userInfo.name1).arg(m_userInfo.name2).arg(m_userInfo.name3));
           QPropertyAnimation *animation = new QPropertyAnimation(this, "windowOpacity");
           animation->setDuration(1000);
           animation->setEasingCurve(QEasingCurve::Linear);
           animation->setStartValue(0);
           animation->setEndValue(1);
           animation->start();
    }
    return QWidget::event(p_event);
}
//--------------------------------------------------------------------------------------------------

void FrmMain::appClose()
{
    close();
}
//--------------------------------------------------------------------------------------------------

FrmMain::~FrmMain()
{
    delete ui;
}
//--------------------------------------------------------------------------------------------------
