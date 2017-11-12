#include "invoice.h"
#include "adding_pages.h"
#include "single_invoice.h"
#include "ui_basetab.h"

#include "QtSql\qsqlerror.h"
#include "QtSql\qsqlquerymodel.h"
#include "QtCore\qdebug.h"
#include "QtWidgets\qlabel.h"
#include "QtWidgets\qlayout.h"
#include "QtCore\QModelIndex"
#include "QtWidgets\qmessagebox.h"

#include <iostream>
#include <regex>

namespace
{
  std::map<size_t, std::pair<std::string, std::string>> tableCols
  {
    { 0,{ "RENR", "Re.Nr." } },
    { 1,{ "REDAT", "Datum" } },
    { 2,{ "KUNR", "Kunde" } },
    { 3,{ "NAME", "Name" } },
    { 4,{ "GESAMT", "Netto" } },
    { 5,{ "BRUTTO", "Brutto" } },
    { 6,{ "ANREDE", "Anrede" } },
    { 7,{ "STRASSE", "Straﬂe" } },
    { 8,{ "ORT", "Ort" } },
    { 9,{ "MGESAMT", "Material" } },
    { 10,{ "LGESAMT", "Leistung" } },
    { 11,{ "SGESAMT", "S-Zeug" } },
    { 12,{ "MWSTGESAMT", "MwstGesamt" } },
    { 13,{ "SKONTO", "Skonto" } },
    { 14,{ "SKBETRAG", "Skonto-Betrag" } },
    { 15,{ "BEZAHLT", "Bezahlt" } },
    { 16,{ "HEADLIN", "Header" } },
    { 17,{ "LIEFDAT", "Lieferdatum" } },
    { 18,{ "SCHLUSS", "Schluss" } },
    { 19,{ "GEDRUCKT", "Gedruckt" } },
    { 20,{ "BETREFF", "Betreff" } },
    { 21,{ "MWSTSATZ", "Mwst" } },
    { 22,{ "KONTOSOLL", "Soll" } },
    { 23,{ "RAB_EXT", "extra Rabatt"}},
    { 24,{ "WEU", "Weu" } }
  };
}


Invoice::Invoice(QWidget *parent)
  : BaseTab(parent)
{
  for (auto &&e : tableCols)
  {
    m_tableFilter[e.second.first] = true;
    if (e.first == 5)
    {
      break;
    }
  }
}

Invoice::~Invoice()
{
}

void Invoice::SetDatabase(QSqlDatabase &db)
{
  m_query = QSqlQuery(db);
  ShowDatabase();
}

void Invoice::ShowDatabase()
{
  std::string sql = "SELECT ";
  for (auto &&s : tableCols)
  {
    if (m_tableFilter[s.second.first])
    {
      if (s.second.first.compare("REDAT") == 0)
      {
        sql += "strftime('%d.%m.%Y', REDAT), ";
      }
      else
      {
        sql += s.second.first + ", ";
      }
    }
    if (s.first == 5)
    {
      break;
    }
  }
  sql = sql.substr(0, sql.size() - 2);
  sql += " FROM RECHNUNG";
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

  m_model->setQuery(m_query);
  size_t idx = 0;
  for (auto &&s : tableCols)
  {
    if (m_tableFilter[s.second.first])
    {
      m_model->setHeaderData((int)idx, Qt::Horizontal, QString::fromStdString(s.second.second));
      idx++;
    }
    if (s.first == 5)
    {
      break;
    }
  }
}

void Invoice::AddEntry()
{
  std::regex lastInvoiceRegex("(\\d+)");
  auto result = std::sregex_iterator(std::begin(m_settings->lastInvoice), std::end(m_settings->lastInvoice), lastInvoiceRegex);
  std::string number = m_settings->lastInvoice.substr(0, 3) + std::to_string(std::stoull(result->begin()->str()) + 1);

  GeneralInputData input;
  input.invoiceNumber = std::stoull(result->begin()->str()) + 1;
  input.currentPrice = 0;

  InvoicePage *page = new InvoicePage(m_settings, input, this);

  if (page->exec() == QDialog::Accepted)
  {
    auto &data = page->data;
    std::string sql = GenerateInsertCommand("RECHNUNG"
      , SqlPair(tableCols[0].first, data.invoiceNumber)
      , SqlPair(tableCols[1].first, data.invoiceDate)
      , SqlPair(tableCols[2].first, data.customerNumber)
      , SqlPair(tableCols[3].first, data.name)
      , SqlPair(tableCols[4].first, 0.0)
      , SqlPair(tableCols[5].first, 0.0)
      , SqlPair(tableCols[6].first, data.salutation)
      , SqlPair(tableCols[7].first, data.street)
      , SqlPair(tableCols[8].first, data.place)
      , SqlPair(tableCols[9].first, 0.0)
      , SqlPair(tableCols[10].first, 0.0)
      , SqlPair(tableCols[11].first, 0.0)
      , SqlPair(tableCols[12].first, 0.0)
      , SqlPair(tableCols[13].first, 0.0)
      , SqlPair(tableCols[14].first, 0.0)
      , SqlPair(tableCols[15].first, 0.0)
      , SqlPair(tableCols[16].first, data.headline)
      , SqlPair(tableCols[17].first, data.deliveryDate)
      , SqlPair(tableCols[18].first, "")
      , SqlPair(tableCols[19].first, "F")
      , SqlPair(tableCols[20].first, data.subject)
      , SqlPair(tableCols[21].first, data.mwst)
      , SqlPair(tableCols[22].first, 0.0)
      , SqlPair(tableCols[23].first, data.discount)
      , SqlPair(tableCols[24].first, 0.0));

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
  
  GeneralInputData input;
  input.invoiceNumber = schl.toLongLong();
  input.currentPrice = profit.toDouble();

  SingleInvoice *page = new SingleInvoice(tableName, input);
  page->SetSettings(m_settings);

  QSqlDatabase invoiceDb = QSqlDatabase::addDatabase("QSQLITE", "invoice");
  invoiceDb.setDatabaseName("invoices.db");
  page->SetDatabase(invoiceDb);

  connect(page, &SingleInvoice::SaveData, [this, &invoiceDb, page]()
  {
    delete page;
    invoiceDb.removeDatabase("invoice");
  });

  connect(page, &SingleInvoice::destroyed, [&invoiceDb]()
  {
    invoiceDb.removeDatabase("invoice");
  });

  connect(page, &SingleInvoice::SaveData, [this]()
  {

  });

  page->show();

  //if (page->exec() == QDialog::Accepted)
  //{
  //  AdressData data = page->data;
  //  std::string sql = "UPDATE Invoice SET" +
  //    std::string(" KUNR = ") + std::to_string(data.number) +
  //    ", ANREDE = '" + data.salutation.toStdString() +
  //    "', NAME = '" + data.name.toStdString() +
  //    "', STRASSE = '" + data.street.toStdString() +
  //    "', PLZ = '" + data.plz.toStdString() +
  //    "', ORT = '" + data.city.toStdString() +
  //    "', TELEFON = '" + data.phone1.toStdString() +
  //    "', FAX = '" + data.fax.toStdString() +
  //    "', TELEFON2 = '" + data.phone2.toStdString() +
  //    "', TELEFON3 = '" + data.phone3.toStdString() +
  //    "', EPUEB = " + std::to_string(data.epUeb) +
  //    " WHERE SUCHNAME = '" + schl.toStdString() + "'";
  //  m_rc = m_query.prepare(QString::fromStdString(sql));
  //  if (!m_rc)
  //  {
  //    qDebug() << m_query.lastError();
  //  }
  //  m_rc = m_query.exec();
  //  if (!m_rc)
  //  {
  //    qDebug() << m_query.lastError();
  //  }
  //  ShowDatabase();
  //}
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
    ShowDatabase();
  }
}

void Invoice::FilterList()
{
  std::map<std::string, std::string> mapping;
  for (auto &&s : tableCols)
  {
    mapping[s.second.first] = s.second.second;
  }
  FilterTable *filter = new FilterTable(m_tableFilter, mapping, this);
  auto backup = m_tableFilter;
  int exec = filter->exec();
  if (exec == QDialog::Accepted)
  {
  }
  else
  {
    m_tableFilter = backup;
  }
  ShowDatabase();
}

void Invoice::PrepareDoc()
{
  auto index = m_ui->databaseView->currentIndex();
  QString id = m_ui->databaseView->model()->data(index.model()->index(index.row(), 0)).toString();
  m_rc = m_query.prepare("SELECT * FROM InvoiceN WHERE SUCHNAME = :ID");
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

  m_doc.clear();
  std::string html = "<table><tr>";
  for (auto &&s : tableCols)
  {
    html += "<th>" + s.second.second + "</th>";
  }
  html += "</tr><tr>";
  for (size_t i = 1; i < tableCols.size(); i++)
  {
    html += "<th>" + m_query.value((int)i).toString().toStdString() + "</td>";
  }
  html += "</tr></table>";
  m_doc.setHtml(QString::fromStdString(html));
}

void Invoice::ExportToPDF()
{
  PrepareDoc();
  m_doc.print(&m_pdfPrinter);
}

void Invoice::PrintEntry()
{
  PrepareDoc();
  BaseTab::EmitToPrinter(m_doc);
}
