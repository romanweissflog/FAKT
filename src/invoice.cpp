#include "invoice.h"
#include "adding_pages.h"
#include "single_entry.h"
#include "ui_basetab.h"

#include "QtSql\qsqlerror.h"
#include "QtSql\qsqlquerymodel.h"
#include "QtCore\qdebug.h"
#include "QtWidgets\qlabel.h"
#include "QtWidgets\qlayout.h"
#include "QtCore\QModelIndex"
#include "QtWidgets\qmessagebox.h"

#include <iostream>

namespace
{
  TabData tabData
  {
    "Invoice",
    "RECHNUNG",
    "RENR",
    PrintType::PrintTypeInvoice,
    {
      { "RENR", "Rechnungs-Nr." },
      { "REDAT", "Datum" },
      { "KUNR", "Kunde" },
      { "NAME", "Name" },
      { "GESAMT", "Netto" },
      { "BRUTTO", "Brutto" },
      { "ANREDE", "Anrede" },
      { "STRASSE", "Straße" },
      { "ORT", "Ort" },
      { "MGESAMT", "Material" },
      { "LGESAMT", "Leistung" },
      { "SGESAMT", "S-Zeug" },
      { "MWSTGESAMT", "MwstGesamt" },
      { "SKONTO", "Skonto" },
      { "SKBETRAG", "Skonto-Betrag" },
      { "BEZAHLT", "Bezahlt" },
      { "HEADLIN", "Header" },
      { "BEZADAT", "Bezahldatum" },
      { "LIEFDAT", "Lieferdatum" },
      { "Z_FRIST_N", "Zahlung normal" },
      { "Z_FRIST_S", "Zahlung Skonto" },
      { "SCHLUSS", "Schluss" },
      { "STUSATZ", "Stundensatz" },
      { "BETREFF", "Betreff" },
      { "MWSTSATZ", "Mwst" }
    },
    { "RENR", "REDAT", "KUNR", "NAME", "GESAMT", "BRUTTO" }
  };
}


Invoice::Invoice(QWidget *parent)
  : BaseTab(tabData, parent)
{
}

Invoice::~Invoice()
{
}

void Invoice::AddEntry()
{
  std::string number = m_settings->lastInvoice;
  InvoicePage *page = new InvoicePage(m_settings, number, TabName::InvoiceTab, this);

  if (page->exec() == QDialog::Accepted)
  {
    auto &data = page->data;
    std::string sql = GenerateInsertCommand("RECHNUNG"
      , SqlPair("RENR", data.baseData->number)
      , SqlPair("REDAT", data.baseData->date)
      , SqlPair("KUNR", data.baseData->customerNumber)
      , SqlPair("NAME", data.baseData->name)
      , SqlPair("GESAMT", 0.0)
      , SqlPair("BRUTTO", 0.0)
      , SqlPair("ANREDE", data.baseData->salutation)
      , SqlPair("STRASSE", data.baseData->street)
      , SqlPair("ORT", data.baseData->place)
      , SqlPair("MGESAMT", 0.0)
      , SqlPair("LGESAMT", 0.0)
      , SqlPair("SGESAMT", 0.0)
      , SqlPair("MWSTGESAMT", 0.0)
      , SqlPair("SKONTO", 0.0)
      , SqlPair("SKBETRAG", 0.0)
      , SqlPair("BEZAHLT", 0.0)
      , SqlPair("HEADLIN", data.baseData->headline)
      , SqlPair("BEZADAT", data.payDate)
      , SqlPair("LIEFDAT", data.deliveryDate)
      , SqlPair("Z_FRIST_N", data.baseData->payNormal)
      , SqlPair("Z_FIRST_S", data.baseData->paySkonto)
      , SqlPair("SCHLUSS", data.baseData->endline)
      , SqlPair("STUSATZ", data.baseData->hourlyRate)
      , SqlPair("BETREFF", data.baseData->subject)
      , SqlPair("MWSTSATZ", data.mwst));

    m_rc = m_query.prepare(QString::fromStdString(sql));
    if (!m_rc)
    {
      qDebug() << m_query.lastError();
    }
    m_rc = m_query.exec();
    if (!m_rc)
    {
      qDebug() << m_query.lastError();
    }
    ShowDatabase();
  }
}

void Invoice::EditEntry()
{
  auto index = m_ui->databaseView->currentIndex();
  if (index.row() == -1 || index.column() == -1)
  {
    return;
  }
  QString schl = m_ui->databaseView->model()->data(index.model()->index(index.row(), 0)).toString();
  QString profit = m_ui->databaseView->model()->data(index.model()->index(index.row(), 2)).toString();
  std::string tableName = std::string("R") + schl.toStdString();

  SingleInvoice *page = new SingleInvoice(tableName);
  page->SetSettings(m_settings);

  QSqlDatabase invoiceDb = QSqlDatabase::addDatabase("QSQLITE", "invoice");
  invoiceDb.setDatabaseName("invoices.db");
  page->SetDatabase(invoiceDb);

  connect(page, &SingleInvoice::SaveData, [this, &invoiceDb, page, tableName]()
  {
    auto &data = page->data;
    std::string sql = GenerateEditCommand("RECHNUNG", "RENR", data.baseData->number.toStdString()
      , SqlPair("GESAMT", data.baseData->total)
      , SqlPair("BRUTTO", data.baseData->brutto)
      , SqlPair("MGESAMT", data.baseData->materialTotal)
      , SqlPair("LGESAMT", data.baseData->serviceTotal)
      , SqlPair("SGESAMT", data.baseData->helperTotal)
      , SqlPair("MWSTGESAMT", data.baseData->mwstTotal)
      , SqlPair("SKONTO", data.baseData->skonto)
      , SqlPair("SKBETRAG", data.skontoTotal));

    delete page;
    invoiceDb.removeDatabase("invoice");
  });

  connect(page, &SingleInvoice::destroyed, [&invoiceDb]()
  {
    invoiceDb.removeDatabase("invoice");
  });

  page->show();
}

void Invoice::DeleteEntry()
{
  QMessageBox *question = new QMessageBox(this);
  question->setWindowTitle("WARNUNG");
  question->setText("Wollen sie den Eintrag entfernen?");
  question->setStandardButtons(QMessageBox::Yes);
  question->addButton(QMessageBox::No);
  question->setDefaultButton(QMessageBox::No);
  if (question->exec() == QMessageBox::Yes)
  {
    auto index = m_ui->databaseView->currentIndex();
    QString id = m_ui->databaseView->model()->data(index.model()->index(index.row(), 0)).toString();
    m_rc = m_query.prepare("DELETE FROM RECHNUNG WHERE RENR = :ID");
    if (!m_rc)
    {
      qDebug() << m_query.lastError();
    }
    m_query.bindValue(":ID", id);
    m_rc = m_query.exec();
    if (!m_rc)
    {
      qDebug() << m_query.lastError();
    }

    QSqlDatabase invoiceDb = QSqlDatabase::addDatabase("QSQLITE", "invoice");
    invoiceDb.setDatabaseName("invoices.db");

    invoiceDb.open();
    QSqlQuery invoiceQuery(invoiceDb);
    m_rc = invoiceQuery.prepare("DROP TABLE IF EXISTS :ID");
    if (!m_rc)
    {
      qDebug() << invoiceQuery.lastError();
    }
    m_query.bindValue(":ID", QString("R") + id);
    m_rc = m_query.exec();
    if (!m_rc)
    {
      qDebug() << invoiceQuery.lastError();
    }
    invoiceDb.close();
    invoiceDb = QSqlDatabase();
    invoiceDb.removeDatabase("invoice");

    ShowDatabase();
  }
}

void Invoice::PrepareDoc(bool withLogo)
{
  auto index = m_ui->databaseView->currentIndex();
  QString id = m_ui->databaseView->model()->data(index.model()->index(index.row(), 0)).toString();
  m_rc = m_query.prepare("SELECT * FROM RECHNUNG WHERE RENR = :ID");
  if (!m_rc)
  {
    qDebug() << m_query.lastError();
  }
  m_query.bindValue(":ID", id);
  m_rc = m_query.exec();
  if (!m_rc)
  {
    qDebug() << m_query.lastError();
  }
  m_rc = m_query.next();
  if (!m_rc)
  {
    qDebug() << m_query.lastError();
  }

  QSqlDatabase dataDb = QSqlDatabase::addDatabase("QSQLITE", "invoice");
  dataDb.setDatabaseName("invoices.db");
  dataDb.open();
  QSqlQuery dataQuery(dataDb);

  QString sql = "SELECT * FROM R" + id;
  m_rc = dataQuery.exec(sql);
  if (!m_rc)
  {
    qDebug() << dataQuery.lastError();
  }

  PrintData printData
  {
    "RECHNUNG",
    m_query.value("ANREDE").toString(),
    m_query.value("NAME").toString(),
    m_query.value("STRASSE").toString(),
    m_query.value("ORT").toString(),
    m_query.value("RENR").toString(),
    m_query.value("REDAT").toString(),
    m_query.value("MWSTSATZ").toDouble(),
    m_query.value("GESAMT").toDouble(),
    m_query.value("MWSTGESAMT").toDouble(),
    m_query.value("BRUTTO").toDouble(),
    m_query.value("HEADLIN").toString(),
    m_query.value("BETREFF").toString(),
    m_query.value("SCHLUSS").toString(),
  };

  m_doc.clear();
  QTextCursor cursor(&m_doc);
  m_export(cursor, printData, dataQuery, withLogo ? m_settings->logoFile : "");
  dataDb = QSqlDatabase();
  dataDb.removeDatabase("invoice");
}

Data* Invoice::GetData(std::string const &artNr)
{
  GeneralMainData *data = new GeneralMainData;
  m_rc = m_query.prepare("SELECT * FROM RECHNUNG WHERE RENR = :ID");
  if (!m_rc)
  {
    qDebug() << m_query.lastError();
  }
  std::string number = artNr.substr(1, artNr.size());
  m_query.bindValue(":ID", QString::fromStdString(number));
  m_rc = m_query.exec();
  if (!m_rc)
  {
    qDebug() << m_query.lastError();
  }
  m_rc = m_query.next();
  if (!m_rc)
  {
    qDebug() << m_query.lastError();
  }

  data->number = m_query.value(1).toString();
  data->date = m_query.value(2).toString();
  data->salutation = m_query.value(3).toString();
  data->name = m_query.value(4).toString();
  data->street = m_query.value(5).toString();
  data->place = m_query.value(6).toString();
  data->materialTotal = m_query.value(7).toDouble();
  data->serviceTotal = m_query.value(8).toDouble();
  data->helperTotal = m_query.value(9).toDouble();
  data->total = m_query.value(10).toDouble();
  data->mwstTotal = m_query.value(11).toDouble();
  data->brutto = m_query.value(12).toDouble();
  data->skonto = m_query.value(13).toDouble();
  data->headline = m_query.value(15).toString();
  data->customerNumber = m_query.value(17).toString();
  data->payNormal = m_query.value(19).toDouble();
  data->paySkonto = m_query.value(20).toDouble();
  data->endline = m_query.value(21).toString();
  data->hourlyRate = m_query.value(22).toDouble();
  data->subject = m_query.value(23).toString();
  return data;
}
