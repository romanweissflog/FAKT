#include "tabs\jobsite.h"
#include "pages\invoice_page.h"
#include "pages\single_jobsite.h"
#include "functionality\sql_helper.hpp"

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
    "Jobsite",
    "BAUSTELLE",
    "Baustellen",
    "RENR",
    PrintType::PrintTypeJobsite,
    {
      { "RENR", "Baust.Nr." },
      { "REDAT", "Datum" },
      { "KUNR", "K.-Nr." },
      { "NAME", "Name" },
      { "GESAMT", "Netto" },
      { "BRUTTO", "Brutto" },
      { "ANREDE", "Anrede" },
      { "STRASSE", QString::fromStdString("Stra" + german::ss + "e") },
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


Jobsite::Jobsite(QWidget *parent)
  : BaseTab(tabData, parent)
{
}

Jobsite::~Jobsite()
{
}

void Jobsite::AddEntry()
{
  QString number = QString::number(std::stoul(m_settings->lastJobsite) + 1);
  InvoicePage *page = new InvoicePage(m_settings, number, TabName::JobsiteTab, this);
  page->hide();
  emit AddSubtab(page, "Baustellen:Neu");
  page->setFocus();
  if (page->exec() == QDialog::Accepted)
  {
    auto data = page->data;
    std::string sql = GenerateInsertCommand("BAUSTELLE"
      , SqlPair("RENR", data->number)
      , SqlPair("REDAT", data->date)
      , SqlPair("KUNR", data->customerNumber)
      , SqlPair("NAME", data->name)
      , SqlPair("GESAMT", 0.0)
      , SqlPair("BRUTTO", 0.0)
      , SqlPair("ANREDE", data->salutation)
      , SqlPair("STRASSE", data->street)
      , SqlPair("ORT", data->place)
      , SqlPair("MGESAMT", 0.0)
      , SqlPair("LGESAMT", 0.0)
      , SqlPair("SGESAMT", 0.0)
      , SqlPair("MWSTGESAMT", 0.0)
      , SqlPair("SKONTO", 0.0)
      , SqlPair("SKBETRAG", 0.0)
      , SqlPair("BEZAHLT", 0.0)
      , SqlPair("HEADLIN", data->headline)
      , SqlPair("BEZADAT", data->payDate)
      , SqlPair("LIEFDAT", data->deliveryDate)
      , SqlPair("Z_FRIST_N", data->payNormal)
      , SqlPair("Z_FRIST_S", data->paySkonto)
      , SqlPair("SCHLUSS", data->endline)
      , SqlPair("STUSATZ", data->hourlyRate)
      , SqlPair("BETREFF", data->subject)
      , SqlPair("MWSTSATZ", data->mwst));

    m_rc = m_query.prepare(QString::fromStdString(sql));
    if (!m_rc)
    {
      qDebug() << m_query.lastError();
    }
    m_rc = m_query.exec();
    if (!m_rc)
    {
      QMessageBox::warning(this, tr("Hinweis"),
        tr("Baustellennummer bereits vergeben - Eintrag wird nicht gespeichert"));
    }
    m_settings->lastJobsite = number.toStdString();
    ShowDatabase();
  }
  emit CloseTab("Baustellen:Neu");
}

void Jobsite::EditEntry()
{
  auto index = m_ui->databaseView->currentIndex();
  if (index.row() == -1 || index.column() == -1)
  {
    return;
  }
  QString schl = m_ui->databaseView->model()->data(index.model()->index(index.row(), 0)).toString();
  QString profit = m_ui->databaseView->model()->data(index.model()->index(index.row(), 2)).toString();
  std::string tableName = std::string("BA") + schl.toStdString();

  SingleJobsite *page = new SingleJobsite(schl.toULongLong(), tableName);
  page->SetSettings(m_settings);
  page->SetDatabase("jobsites.db");
  page->SetLastData(GetData(schl.toStdString()).get());

  page->hide();
  emit AddSubtab(page, "Baustellen:" + schl);

  connect(page, &SingleJobsite::UpdateData, [this, page, tableName]()
  {
    auto data = page->data;
    std::string sql = GenerateEditCommand("BAUSTELLE", "RENR", data->number.toStdString()
      , SqlPair("GESAMT", data->total)
      , SqlPair("BRUTTO", data->brutto)
      , SqlPair("MGESAMT", data->materialTotal)
      , SqlPair("LGESAMT", data->serviceTotal)
      , SqlPair("SGESAMT", data->helperTotal)
      , SqlPair("MWSTGESAMT", data->mwstTotal)
      , SqlPair("SKONTO", data->skonto)
      , SqlPair("SKBETRAG", data->skontoTotal));

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
  });
}

void Jobsite::DeleteEntry()
{
  auto index = m_ui->databaseView->currentIndex();
  if (index.row() == -1 || index.column() == -1)
  {
    return;
  }
  QMessageBox *question = util::GetDeleteMessage(this);
  if (question->exec() == QMessageBox::Yes)
  {
    auto index = m_ui->databaseView->currentIndex();
    QString id = m_ui->databaseView->model()->data(index.model()->index(index.row(), 0)).toString();
    m_rc = m_query.prepare("DELETE FROM BAUSTELLE WHERE RENR = :ID");
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

    QSqlDatabase invoiceDb = QSqlDatabase::addDatabase("QSQLITE", "jobsite");
    invoiceDb.setDatabaseName("jobsites.db");

    invoiceDb.open();
    QSqlQuery invoiceQuery(invoiceDb);
    m_rc = invoiceQuery.prepare("DROP TABLE IF EXISTS BA" + id);
    if (!m_rc)
    {
      qDebug() << invoiceQuery.lastError();
    }
    m_rc = invoiceQuery.exec();
    if (!m_rc)
    {
      qDebug() << invoiceQuery.lastError();
    }
    invoiceDb.close();
    invoiceDb = QSqlDatabase();
    invoiceDb.removeDatabase("jobsite");

    ShowDatabase();
  }
}

ReturnValue Jobsite::PrepareDoc(bool withLogo)
{
  auto index = m_ui->databaseView->currentIndex();
  if (index.row() == -1 || index.column() == -1)
  {
    return ReturnValue::ReturnFailure;
  }

  QString id = m_ui->databaseView->model()->data(index.model()->index(index.row(), 0)).toString();
  m_rc = m_query.prepare("SELECT * FROM BAUSTELLE WHERE RENR = :ID");
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

  QSqlDatabase dataDb = QSqlDatabase::addDatabase("QSQLITE", "jobsite");
  dataDb.setDatabaseName("jobsites.db");
  dataDb.open();
  QSqlQuery dataQuery(dataDb);

  QString sql = "SELECT * FROM BA" + id;
  m_rc = dataQuery.exec(sql);
  if (!m_rc)
  {
    qDebug() << dataQuery.lastError();
  }

  PrintData printData
  {
    TabName::JobsiteTab,
    "BAUSTELLE",
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
  ReturnValue rv = m_export(cursor, printData, dataQuery, withLogo ? m_settings->logoFile : "");

  dataDb = QSqlDatabase();
  dataDb.removeDatabase("jobsite");

  return rv;
}

std::unique_ptr<Data> Jobsite::GetData(std::string const &artNr)
{
  std::unique_ptr<InvoiceData> data(new InvoiceData());
  m_rc = m_query.prepare("SELECT * FROM BAUSTELLE WHERE RENR = :ID");
  if (!m_rc)
  {
    qDebug() << m_query.lastError();
  }
  m_query.bindValue(":ID", QString::fromStdString(artNr));
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
  data->skontoTotal = m_query.value(14).toDouble();
  data->paid = m_query.value(15).toDouble();
  data->headline = m_query.value(16).toString();
  data->payDate = m_query.value(17).toString();
  data->customerNumber = m_query.value(18).toString();
  data->payNormal = m_query.value(19).toDouble();
  data->paySkonto = m_query.value(20).toDouble();
  data->endline = m_query.value(21).toString();
  data->hourlyRate = m_query.value(22).toDouble();
  data->subject = m_query.value(23).toString();
  return data;
}

void Jobsite::SetData(std::unique_ptr<Data> &input)
{
  std::unique_ptr<InvoiceData> data(static_cast<InvoiceData*>(input.release()));
  std::string sql = GenerateEditCommand("BAUSTELLE", "RENR", data->number.toStdString()
    , SqlPair("RENR", data->number)
    , SqlPair("REDAT", data->date)
    , SqlPair("KUNR", data->customerNumber)
    , SqlPair("NAME", data->name)
    , SqlPair("GESAMT", 0.0)
    , SqlPair("BRUTTO", 0.0)
    , SqlPair("ANREDE", data->salutation)
    , SqlPair("STRASSE", data->street)
    , SqlPair("ORT", data->place)
    , SqlPair("MGESAMT", 0.0)
    , SqlPair("LGESAMT", 0.0)
    , SqlPair("SGESAMT", 0.0)
    , SqlPair("MWSTGESAMT", 0.0)
    , SqlPair("SKONTO", 0.0)
    , SqlPair("SKBETRAG", 0.0)
    , SqlPair("BEZAHLT", 0.0)
    , SqlPair("HEADLIN", data->headline)
    , SqlPair("BEZADAT", data->payDate)
    , SqlPair("LIEFDAT", data->deliveryDate)
    , SqlPair("Z_FRIST_N", data->payNormal)
    , SqlPair("Z_FRIST_S", data->paySkonto)
    , SqlPair("SCHLUSS", data->endline)
    , SqlPair("STUSATZ", data->hourlyRate)
    , SqlPair("BETREFF", data->subject)
    , SqlPair("MWSTSATZ", data->mwst));

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
