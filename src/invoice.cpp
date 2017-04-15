#include "invoice.h"
#include "adding_pages.h"
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
    { 12,{ "MWTGESAMT", "MwstGesamt" } },
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
    { 23,{ "WEU", "Weu" } }
  };

  std::map<size_t, std::pair<std::string, std::string>> invoiceTableCols
  {
    { 0, { "POSIT", "Pos" } },
    { 1, { "ARTNR", "Art.-Nr." } },
    { 2, { "ARTBEZ", "Bezeichnung" } },
    { 3, { "MENGE", "Menge" } },
    { 4, { "EP", "EP" } },
    { 5, { "GP", "GP" } },
    { 6, { "ME", "Einheit" } },
    { 7, { "SP", "SP" } },
    { 8, { "BAUZEIT", "Bauzeit" } },
    { 9, { "P_RABATT", "Rabatt" } },
    { 10, { "EKP", "EKP" } },
    { 11, { "MULTI", "Aufschlag" } },
    { 12, { "LPKORR", "Korrelation" } }
  };
}


SingleInvoice::SingleInvoice(std::string const &tableName, QWidget *parent)
  : BaseTab(parent)
  , m_tableName(tableName)
{
  this->setWindowTitle("Rechnung");

  for (auto &&e : invoiceTableCols)
  {
    m_tableFilter[e.second.first] = true;
    if (e.first == 5)
    {
      break;
    }
  }
}

SingleInvoice::~SingleInvoice()
{
  if (m_db.isOpen())
  {
    m_db.close();
  }
}

void SingleInvoice::SetDatabase(QSqlDatabase &db)
{
  m_db = db;
  m_db.open();
  m_query = QSqlQuery(m_db);

  std::string sql = "CREATE TABLE IF NOT EXISTS " + m_tableName
    + "(id INTEGER PRIMARY KEY, ";
  for (auto &&h : invoiceTableCols)
  {
    sql += h.second.first + " TEXT, ";
  }
  sql = sql.substr(0, sql.size() - 2);
  sql += ");";
  m_rc = m_query.exec(QString::fromStdString(sql));
  if (!m_rc)
  {
    qDebug() << m_query.lastError();
  }

  connect(m_ui->databaseView, &QTableView::doubleClicked, this, &BaseTab::EditEntry);

  ShowDatabase();
}

void SingleInvoice::ShowDatabase()
{
  std::string sql = "SELECT ";
  for (auto &&s : invoiceTableCols)
  {
    if (m_tableFilter[s.second.first])
    {
      sql += s.second.first + ", ";
    }
    if (s.first == 5)
    {
      break;
    }
  }
  sql = sql.substr(0, sql.size() - 2);
  sql += " FROM " + m_tableName;
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
  for (auto &&s : invoiceTableCols)
  {
    if (m_tableFilter[s.second.first])
    {
      m_model->setHeaderData(idx, Qt::Horizontal, QString::fromStdString(s.second.second));
      idx++;
    }
    if (s.first == 5)
    {
      break;
    }
  }
}

void SingleInvoice::AddEntry()
{
  GeneralPage *page = new GeneralPage(m_settings, m_query, m_input, this);
  if (page->exec() == QDialog::Accepted)
  {
    //auto &data = page->data;
    //std::string sql = "INSERT INTO LEISTUNG (";
    //for (auto &&s : tableCols)
    //{
    //  sql += s.second.first + ", ";
    //}
    //sql = sql.substr(0, sql.size() - 2);
    //sql += ") VALUES ('" + data.key.toStdString() + "', '" +
    //  data.description.toStdString() + "', '" +
    //  data.unit.toStdString() + "', " +
    //  std::to_string(data.netto) + ", " +
    //  std::to_string(data.brutto) + ", " +
    //  std::to_string(data.ekp) + ", " +
    //  std::to_string(data.ep) + ", '" +
    //  data.supplier.toStdString() + "', " +
    //  std::to_string(data.minutes) + ", ";
    //std::to_string(data.stockSize) + ")";
    //m_rc = m_query.prepare(QString::fromStdString(sql));
    //if (!m_rc)
    //{
    //  qDebug() << m_query.lastError();
    //}
    //m_rc = m_query.exec();
    //if (!m_rc)
    //{
    //  qDebug() << m_query.lastError();
    //}
    ShowDatabase();
  }
}

void SingleInvoice::DeleteEntry()
{}

void SingleInvoice::EditEntry()
{}

void SingleInvoice::FilterList()
{}


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
      m_model->setHeaderData(idx, Qt::Horizontal, QString::fromStdString(s.second.second));
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
  SingleInvoice *page = new SingleInvoice(number);
  page->SetSettings(m_settings);
  QSqlDatabase invoiceDb = QSqlDatabase::addDatabase("QSQLITE", "invoice");
  invoiceDb.setDatabaseName("invoices.db");
  page->SetDatabase(invoiceDb);
  page->show();
}

void Invoice::EditEntry()
{
  auto index = m_ui->databaseView->currentIndex();
  if (index.row() == -1 || index.column() == -1)
  {
    return;
  }
  QString schl = m_ui->databaseView->model()->data(index.model()->index(index.row(), 0)).toString();

  AdressPage *page = new AdressPage(m_settings, m_query, schl, this);
  if (page->exec() == QDialog::Accepted)
  {
    AdressData data = page->data;
    std::string sql = "UPDATE InvoiceN SET" +
      std::string(" KUNR = ") + std::to_string(data.number) +
      ", ANREDE = '" + data.salutation.toStdString() +
      "', NAME = '" + data.name.toStdString() +
      "', STRASSE = '" + data.street.toStdString() +
      "', PLZ = '" + data.plz.toStdString() +
      "', ORT = '" + data.city.toStdString() +
      "', TELEFON = '" + data.phone1.toStdString() +
      "', FAX = '" + data.fax.toStdString() +
      "', TELEFON2 = '" + data.phone2.toStdString() +
      "', TELEFON3 = '" + data.phone3.toStdString() +
      "', EPUEB = " + std::to_string(data.epUeb) +
      " WHERE SUCHNAME = '" + schl.toStdString() + "'";
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
    html += "<th>" + m_query.value(i).toString().toStdString() + "</td>";
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
