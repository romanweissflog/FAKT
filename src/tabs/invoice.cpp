#include "tabs\invoice.h"
#include "pages\invoice_page.h"
#include "pages\single_invoice.h"
#include "functionality\sql_helper.hpp"
#include "tabs\payment.h"

#include "ui_basetab.h"

#include "QtSql\qsqlerror.h"
#include "QtSql\qsqlquerymodel.h"
#include "QtCore\qdebug.h"
#include "QtWidgets\qlabel.h"
#include "QtWidgets\qlayout.h"
#include "QtCore\QModelIndex"
#include "QtWidgets\qmessagebox.h"
#include "QtWidgets\qshortcut.h"

#include <iostream>

namespace
{
  TabData tabData
  {
    TabName::InvoiceTab,
    "Invoice",
    "RECHNUNG",
    "R",
    "invoices.db",
    "Rechnungen",
    "RENR",
    printmask::Invoice,
    {
      { "RENR", "Rechnungs-Nr." },
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
      { "RABATT", "Rabatt" },
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
  QPushButton *payment = new QPushButton("Zahlungseingang (Z)", this);
  connect(payment, &QPushButton::clicked, this, &Invoice::OpenPayment);
  m_ui->layoutAction->addWidget(payment);
  new QShortcut(QKeySequence(Qt::Key_Z), this, SLOT(OpenPayment()));
}

Invoice::~Invoice()
{
}

void Invoice::AddEntry()
{
  QString number = QString::number(std::stoul(m_settings->lastInvoice) + 1);
  InvoicePage *page = new InvoicePage(m_settings, number, TabName::InvoiceTab, this);
  connect(page, &PageFramework::AddExtraPage, [this, page](QWidget *widget, QString const &txt)
  {
    emit AddSubtab(widget, "Rechnungen:Neu:" + txt);
  });
  connect(page, &PageFramework::CloseExtraPage, [this, page](QString const &txt)
  {
    emit CloseTab("Rechnungen:Neu:" + txt);
  });
  emit AddSubtab(page, "Rechnungen:Neu");
  connect(page, &PageFramework::Accepted, [this, page]()
  {
    try
    {
      auto data = page->content->data;
      std::string sql = GenerateInsertCommand("RECHNUNG"
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
        , SqlPair("SKONTO", data->skonto)
        , SqlPair("SKBETRAG", 0.0)
        , SqlPair("BEZAHLT", 0.0)
        , SqlPair("HEADLIN", data->headline)
        , SqlPair("BEZADAT", data->payDate)
        , SqlPair("LIEFDAT", data->deliveryDate)
        , SqlPair("RABATT", data->discount)
        , SqlPair("Z_FRIST_N", data->payNormal)
        , SqlPair("Z_FRIST_S", data->paySkonto)
        , SqlPair("SCHLUSS", data->endline)
        , SqlPair("STUSATZ", data->hourlyRate)
        , SqlPair("BETREFF", data->subject)
        , SqlPair("MWSTSATZ", data->mwst));

      m_rc = m_query.prepare(QString::fromStdString(sql));
      if (!m_rc)
      {
        throw std::runtime_error(m_query.lastError().text().toStdString());
        return;
      }
      m_rc = m_query.exec();
      if (!m_rc)
      {
        QMessageBox::warning(this, tr("Hinweis"),
          tr("Rechnungsnummer bereits vergeben - Eintrag wird nicht gespeichert"));
      }
      m_settings->lastInvoice = data->number.toStdString();
      ShowDatabase();
    }
    catch (std::runtime_error e)
    {
      Log::GetLog().Write(LogType::LogTypeError, m_logId, e.what());
    }
    emit CloseTab("Rechnungen:Neu");
  });
  connect(page, &PageFramework::Declined, [&]()
  {
    emit CloseTab("Rechnungen:Neu");
  });
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

  SingleInvoice *page = new SingleInvoice(schl.toULongLong());
  page->SetSettings(m_settings);
  page->SetDatabase("invoices.db");
  auto data = GetData(schl.toStdString());
  if (!data)
  {
    return;
  }
  page->SetLastData(data.get());

  page->hide();
  emit AddSubtab(page, "Rechnungen:" + schl);

  connect(page, &SingleInvoice::UpdateData, [this, page, tableName]()
  {
    auto data = page->data;
    std::string sql = GenerateEditCommand("RECHNUNG", "RENR", data->number.toStdString()
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
      Log::GetLog().Write(LogType::LogTypeError, m_logId, m_query.lastError().text().toStdString());
      return;
    }
    m_rc = m_query.exec();
    if (!m_rc)
    {
      Log::GetLog().Write(LogType::LogTypeError, m_logId, m_query.lastError().text().toStdString());
      return;
    }
    ShowDatabase();
  });
}

void Invoice::DeleteDataTable(QString const &key)
{
  QSqlDatabase invoiceDb = QSqlDatabase::addDatabase("QSQLITE", "invoice");
  invoiceDb.setDatabaseName("invoices.db");

  invoiceDb.open();
  QSqlQuery invoiceQuery(invoiceDb);
  QString const invoiceId = util::GetPaddedNumber(key);
  m_rc = invoiceQuery.prepare("DROP TABLE IF EXISTS R" + invoiceId);
  if (!m_rc)
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, invoiceQuery.lastError().text().toStdString());
    return;
  }
  m_rc = invoiceQuery.exec();
  if (!m_rc)
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, invoiceQuery.lastError().text().toStdString());
    return;
  }
  invoiceDb.close();
  invoiceDb = QSqlDatabase();
  invoiceDb.removeDatabase("invoice");
}

std::unique_ptr<Data> Invoice::GetData(std::string const &artNr)
{
  std::unique_ptr<InvoiceData> data(new InvoiceData());
  m_rc = m_query.prepare("SELECT * FROM RECHNUNG WHERE RENR = :ID");
  if (!m_rc)
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, m_query.lastError().text().toStdString());
    return std::unique_ptr<Data>();
  }
  m_query.bindValue(":ID", QString::fromStdString(artNr));
  m_rc = m_query.exec();
  if (!m_rc)
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, m_query.lastError().text().toStdString());
    return std::unique_ptr<Data>();
  }
  m_rc = m_query.next();
  if (!m_rc)
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, m_query.lastError().text().toStdString());
    return std::unique_ptr<Data>();
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
  data->deliveryDate = m_query.value(19).toString();
  data->discount = m_query.value(20).toDouble();
  data->payNormal = m_query.value(21).toDouble();
  data->paySkonto = m_query.value(22).toDouble();
  data->endline = m_query.value(23).toString();
  data->hourlyRate = m_query.value(24).toDouble();
  data->subject = m_query.value(25).toString();
  data->mwst = m_query.value(26).toDouble();
  return data;
}

void Invoice::SetData(Data *input)
{
  InvoiceData *data = static_cast<InvoiceData*>(input);
  std::string sql = GenerateEditCommand("RECHNUNG", "RENR", data->number.toStdString()
    , SqlPair("RENR", data->number)
    , SqlPair("REDAT", data->date)
    , SqlPair("KUNR", data->customerNumber)
    , SqlPair("NAME", data->name)
    , SqlPair("GESAMT", data->total)
    , SqlPair("BRUTTO", data->brutto)
    , SqlPair("ANREDE", data->salutation)
    , SqlPair("STRASSE", data->street)
    , SqlPair("ORT", data->place)
    , SqlPair("MGESAMT", data->materialTotal)
    , SqlPair("LGESAMT", data->serviceTotal)
    , SqlPair("SGESAMT", data->helperTotal)
    , SqlPair("MWSTGESAMT", data->mwstTotal)
    , SqlPair("SKONTO", data->skonto)
    , SqlPair("SKBETRAG", data->skontoTotal)
    , SqlPair("BEZAHLT", data->paid)
    , SqlPair("HEADLIN", data->headline)
    , SqlPair("BEZADAT", data->payDate)
    , SqlPair("LIEFDAT", data->deliveryDate)
    , SqlPair("RABATT", data->discount)
    , SqlPair("Z_FRIST_N", data->payNormal)
    , SqlPair("Z_FRIST_S", data->paySkonto)
    , SqlPair("SCHLUSS", data->endline)
    , SqlPair("STUSATZ", data->hourlyRate)
    , SqlPair("BETREFF", data->subject)
    , SqlPair("MWSTSATZ", data->mwst));
  
  m_rc = m_query.prepare(QString::fromStdString(sql));
  if (!m_rc)
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, m_query.lastError().text().toStdString());
    return;
  }
  m_rc = m_query.exec();
  if (!m_rc)
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, m_query.lastError().text().toStdString());
    return;
  }
  ShowDatabase();
}

void Invoice::OpenPayment()
{
  Payment *payment = new Payment(m_query, this); 
  payment->SetSettings(m_settings);
  emit AddSubtab(payment, "Rechnungen:Zahlungen");
  payment->ShowDatabase();
}
