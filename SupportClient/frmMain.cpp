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

    model = new QSqlQueryModel();
    ui->tblGridHistory->setModel(model);

    ui->btnCallAdd->setIcon(QIcon(":/images/arrowright.png"));
    ui->btnClose->setIcon(QIcon(":/images/exit.png"));
    ui->actionClose->setIcon(QIcon(":/images/exit.png"));
    ui->btnCallSave->setIcon(QIcon(":/images/add.png"));
    ui->actionAbout->setIcon(QIcon(":/images/about.png"));
    ui->actionSaveToXML->setIcon(QIcon(":/images/xml_import.png"));
    ui->actionLoadFromXML->setIcon(QIcon(":/images/xml_export.png"));
    ui->btnMoveItem->setIcon(QIcon(":/images/arrowright.png"));


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

    udpSock = new QUdpSocket(this);
    udpSock->bind(8091);
    connect(udpSock, SIGNAL(readyRead()), SLOT(processDatagrams()));

    modelExtInfo = new QStandardItemModel;
}
//----------------------------------------------------------------------------------------------------------------------

void FrmMain::initDB(QString dbName)
{
    db = QSqlDatabase::database();
    db.setDatabaseName(dbName);
    if (!db.open()) {
         QMessageBox::warning(this, tr("Error opening DB!"), db.lastError().driverText());
     } else {
         refreshTable();
     }
}
//----------------------------------------------------------------------------------------------------------------------

void FrmMain::refreshTable()
{
    QSqlQuery query;

    if (ui->edtInsertCallPhone->text() != "") {
        query.prepare("select name, phone, call_d_begin, question, answer, user_r, name1, name2, name3 from v_calls where user_r=:user and phone like :phone;");
        query.bindValue(":user", userInfo.user);
        query.bindValue(":phone", ui->edtInsertCallPhone->text() + '%');
    } else {
        query.prepare("select name, phone, call_d_begin, question, answer, user_r, name1, name2, name3 from v_calls where user_r=:user;");
        query.bindValue(":user", userInfo.user);
    }

    if (query.exec()) {
        model->setQuery(query);
        model->setHeaderData(0, Qt::Horizontal, tr("Full name"));
        model->setHeaderData(1, Qt::Horizontal, tr("Phone"));
        model->setHeaderData(2, Qt::Horizontal, tr("Date"));

        if (model->lastError().isValid()) {
            qDebug() << model->lastError();
            QMessageBox::warning(this, tr("Error data model!"), model->lastError().driverText());
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
//----------------------------------------------------------------------------------------------------------------------

void FrmMain::callAdd()
{
    if (ui->edtInsertCallPhone->text() == "") {
        QMessageBox::warning(this, tr("Error!"), tr("No phone number of incoming call."));
    } else {
        ui->edtPhone->setText(ui->edtInsertCallPhone->text());
        ui->edtDBegin->setText(QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm:ss"));
        ui->edtInsertCallPhone->clear();
        ui->btnCallAdd->setEnabled(false);
        ui->btnCallSave->setEnabled(true);
    }

}
//----------------------------------------------------------------------------------------------------------------------

void FrmMain::callSave()
{
    if (ui->edtPhone->text() == "") {
        QMessageBox::warning(this, tr("Error!"), tr("No phone number of incoming call. Saving is not possible"));
        return;
    }
    QSqlQuery query;
    query.prepare("INSERT INTO calls VALUES (:user, :name1 , :name2, :name3, :phone, :question, :answer, :call_d_begin, :call_d_end);");
    query.bindValue(":user", userInfo.user);
    query.bindValue(":name1", ui->edtName1->text());
    query.bindValue(":name2", ui->edtName2->text());
    query.bindValue(":name3", ui->edtName3->text());
    query.bindValue(":phone", ui->edtPhone->text());
    query.bindValue(":question", ui->tedtQuestion->toPlainText());
    query.bindValue(":answer", ui->tedtAnswer->toPlainText());
    query.bindValue(":call_d_begin", ui->edtDBegin->text());
    query.bindValue(":call_d_end", QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm:ss"));

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
         QMessageBox::warning(this, tr("Error!"), tr("Error execution sql query!") + "\n" + query.lastError().driverText());
    }
}
//----------------------------------------------------------------------------------------------------------------------

void FrmMain::moveItem()
{
    QItemSelectionModel *select = ui->tblGridHistory->selectionModel();
    if (select->hasSelection()) {
        int rowIdx = select->currentIndex().row();

        ui->edtName1->setText(model->record(rowIdx).value("name1").toString());
        ui->edtName2->setText(model->record(rowIdx).value("name2").toString());
        ui->edtName3->setText(model->record(rowIdx).value("name3").toString());
    }
}
//----------------------------------------------------------------------------------------------------------------------


void FrmMain::selectionChanged()
{
    QItemSelectionModel *select = ui->tblGridHistory->selectionModel();
    if (select->hasSelection()) {
        int rowIdx = select->currentIndex().row();

        QStandardItem *itemQuestion;
        QStandardItem *itemAnswer;
        QStringList horizontalHeader;

        modelExtInfo->clear();

        itemQuestion = new QStandardItem(QString(model->record(rowIdx).value("question").toString()));
        itemAnswer = new QStandardItem(QString(model->record(rowIdx).value("answer").toString()));
        modelExtInfo->setItem(0, 0, itemQuestion);
        modelExtInfo->setItem(0, 1, itemAnswer);

        horizontalHeader.append(tr("Question"));
        horizontalHeader.append(tr("Answer"));
        modelExtInfo->setHorizontalHeaderLabels(horizontalHeader);

        ui->tblExtendedInfo->setModel(modelExtInfo);
        ui->tblExtendedInfo->setColumnWidth(0, 190);
        ui->tblExtendedInfo->setColumnWidth(1, 190);
    }
}
//----------------------------------------------------------------------------------------------------------------------

void FrmMain::changedInsertCall()
{
    refreshTable();
}
//----------------------------------------------------------------------------------------------------------------------

void FrmMain::loadFromXML()
{
    QString fileName;

    fileName = QFileDialog::getOpenFileName(this, tr("Open file"), QDir::currentPath(),tr("XML (*.xml)"));
    if (fileName != "") {
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
//----------------------------------------------------------------------------------------------------------------------

void FrmMain::eachNode(const QDomNode& node)
{
   QDomNode domNode = node.firstChild();
   while(!domNode.isNull()) {
       if(domNode.isElement()) {
          QDomElement domElement = domNode.toElement();
          if(!domElement.isNull()) {
              if(domElement.tagName() == "callInfo") {
                  addItemToDB(domElement);
              }
          }
       }
       eachNode(domNode);
       domNode = domNode.nextSibling();
    }
}
//----------------------------------------------------------------------------------------------------------------------

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
                if (fields != "") {
                    fields += "," + domElement.tagName();
                    values += ",'" + domElement.text() + "'";
                } else {
                    fields = domElement.tagName();
                    values = "'" + domElement.text() + "'";
                }
           }
        }
        eachNode(domNode);
        domNode = domNode.nextSibling();
    }

    sqlTxt = "insert into calls (" + fields + ") values (" + values + ");";
    query.prepare(sqlTxt);
    if (!query.exec()) {
        QMessageBox::warning(this, tr("Error!"), tr("SQL: ") + sqlTxt + "\nDB_Error: " + db.lastError().driverText());
    }
    refreshTable();
}
//----------------------------------------------------------------------------------------------------------------------

void FrmMain::saveToXML()
{
    QString fileName;

    fileName = QFileDialog::getSaveFileName(this, tr("Save file"), QDir::currentPath(), tr("XML (*.xml)"));
    if (fileName != "") {
        QDomDocument resultXML = xmlCreate();

        QFile file(fileName);
        if(file.open(QIODevice::WriteOnly)) {
            QTextStream(&file) << resultXML.toString();
            file.close();
            QMessageBox::information(this, tr("Unload to XML!"), tr("Unload to XML completed."));
        }
    }
}
//----------------------------------------------------------------------------------------------------------------------

QDomDocument FrmMain::xmlCreate()
{
    QDomDocument doc("support");
    QDomElement  domElement = doc.createElement("calls");
    QSqlQuery query;

    doc.appendChild(domElement);

    if (query.exec("select * from calls;")) {
        while (query.next()) {
            QDomElement itemCall = xmlCreateItemCall(
                        doc,
                        query.value("user_r").toString(),
                        query.value("name1").toString(),
                        query.value("name2").toString(),
                        query.value("name3").toString(),
                        query.value("phone").toString(),
                        query.value("question").toString(),
                        query.value("answer").toString(),
                        query.value("call_d_begin").toString(),
                        query.value("call_d_end").toString()
                        );
            domElement.appendChild(itemCall);
        }
    }

    return doc;
}
//----------------------------------------------------------------------------------------------------------------------

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

    QDomElement domElement = xmlMakeElement(domDoc, "callInfo", QString().setNum(nNumber));

    domElement.appendChild(xmlMakeElement(domDoc, "user_r", "", strUser));
    domElement.appendChild(xmlMakeElement(domDoc, "name1", "", strName1));
    domElement.appendChild(xmlMakeElement(domDoc, "name2", "", strName2));
    domElement.appendChild(xmlMakeElement(domDoc, "name3", "", strName3));
    domElement.appendChild(xmlMakeElement(domDoc, "phone", "", strPhone));
    domElement.appendChild(xmlMakeElement(domDoc, "question", "", strQuestion));
    domElement.appendChild(xmlMakeElement(domDoc, "answer", "", strAnswer));
    domElement.appendChild(xmlMakeElement(domDoc, "call_d_begin", "", strCallDBegin));
    domElement.appendChild(xmlMakeElement(domDoc, "call_d_end", "", strCallDEnd));

    nNumber++;

    return domElement;
}
//----------------------------------------------------------------------------------------------------------------------

QDomElement FrmMain::xmlMakeElement(
         QDomDocument& domDoc,
         const QString& strName,
         const QString& strAttr,
         const QString& strText
         )
{
    QDomElement domElement = domDoc.createElement(strName);

    if (!strAttr.isEmpty()) {
        QDomAttr domAttr = domDoc.createAttribute("number");
        domAttr.setValue(strAttr);
        domElement.setAttributeNode(domAttr);
    }

    if (!strText.isEmpty()) {
        QDomText domText = domDoc.createTextNode(strText);
        domElement.appendChild(domText);
    }
    return domElement;
}
//----------------------------------------------------------------------------------------------------------------------

void FrmMain::processDatagrams()
{
    QByteArray baDatagram;

    do {
        baDatagram.resize(udpSock->pendingDatagramSize());
        udpSock->readDatagram(baDatagram.data(), baDatagram.size());
    } while(udpSock->hasPendingDatagrams());

    QString callPhone;
    QDataStream dataStream(&baDatagram, QIODevice::ReadOnly);

    dataStream.setVersion(QDataStream::Qt_5_5);
    dataStream >> callPhone;

    ui->edtInsertCallPhone->setText(callPhone);
}
//----------------------------------------------------------------------------------------------------------------------

void FrmMain::aboutShow()
{
    FrmAbout *fAbout;

    fAbout = new FrmAbout();
    fAbout->show();
}
//----------------------------------------------------------------------------------------------------------------------

void FrmMain::keyPressEvent(QKeyEvent* evnt)
{
    if (evnt->modifiers()==Qt::CTRL) {
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
//----------------------------------------------------------------------------------------------------------------------

bool FrmMain::event(QEvent *p_event)
{
    if( p_event->type() == QEvent::ShowToParent) {
           setWindowTitle(windowTitle() + QString(" - %1 %2 %3").arg(userInfo.name1).arg(userInfo.name2).arg(userInfo.name3));
           QPropertyAnimation *animation = new QPropertyAnimation(this, "windowOpacity");
           animation->setDuration(1000);
           animation->setEasingCurve(QEasingCurve::Linear);
           animation->setStartValue(0);
           animation->setEndValue(1);
           animation->start();
    }
    return QWidget::event(p_event);
}
//----------------------------------------------------------------------------------------------------------------------

void FrmMain::appClose()
{
    close();
}
//----------------------------------------------------------------------------------------------------------------------

FrmMain::~FrmMain()
{
    delete ui;
}
//----------------------------------------------------------------------------------------------------------------------
