#include "address.h"
#include "adding_pages.h"
#include "ui_basetab.h"
#include "sql_helper.hpp"

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
  std::map<size_t, std::pair<std::string, std::string>> tableCols
  {
    { 0,  { "SUCHNAME", "Suchname" } },
    { 1,  { "TELEFON", "Telefon" } },
    { 2,  { "KUNR", "K.-Nummer" } },
    { 3,  { "NAME", "Name" } },
    { 4,  { "PLZ", "PLZ" } },
    { 5,  { "ORT", "Ort" } },
    { 6,  { "STRASSE", "Straﬂe" } },
    { 7,  { "ANREDE", "Anrede" } },
    { 8,  { "FAX", "Fax" } },
    { 9,  { "Q1", "Q1" } },
    { 10, { "Q2", "Q2" } },
    { 11, { "Q3", "Q3" } },
    { 12, { "Q4", "Q4" } },
    { 13, { "JAHR", "Jahr" } },
    { 14, { "GESUMS", "Gesamt" } },
    { 15, { "OPSUMME", "OffeneSumme" } },
    { 16, { "TELEFON2", "Telefon2" } },
    { 17, { "TELEFON3", "Telefon3" } },
    { 18, { "EPUEB", "EP-‹bernahme" } }
  };
}


Address::Address(QWidget *parent)
  : BaseTab(parent)
{
  for (auto &&e : tableCols)
  {
    if (e.second.first.compare("ANREDE") == 0)
    {
      break;
    }
    m_tableFilter[e.second.first] = true;
  }
}

Address::~Address()
{
}

void Address::SetDatabase(QSqlDatabase &db)
{
  m_query = QSqlQuery(db);
  ShowDatabase();
}

void Address::ShowDatabase()
{
  std::string sql = "SELECT ";
  for (auto &&s : tableCols)
  {
    if (s.second.first.compare("ANREDE") == 0)
    {
      break;
    }
    if (m_tableFilter[s.second.first])
    {
      sql += s.second.first + ", ";
    }
  }
  sql = sql.substr(0, sql.size() - 2);
  sql += " FROM ADRESSEN";
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
    if (s.second.first.compare("ANREDE") == 0)
    {
      break;
    }
    if (m_tableFilter[s.second.first])
    {
      m_model->setHeaderData(idx, Qt::Horizontal, QString::fromStdString(s.second.second));
      idx++;
    }
  }
}

void Address::AddEntry()
{ 
  AddressPage *page = new AddressPage(m_settings, m_query, "", this);
  if (page->exec() == QDialog::Accepted)
  {
    auto &data = page->data;

    std::string sql = GenerateInsertCommand(std::string("ADRESSEN")
      , SqlPair(tableCols[0].first, data.key)
      , SqlPair(tableCols[1].first, data.phone1)
      , SqlPair(tableCols[2].first, data.number)
      , SqlPair(tableCols[3].first, data.name)
      , SqlPair(tableCols[4].first, data.plz)
      , SqlPair(tableCols[5].first, data.city)
      , SqlPair(tableCols[6].first, data.street)
      , SqlPair(tableCols[7].first, data.salutation)
      , SqlPair(tableCols[8].first, data.fax)
      , SqlPair(tableCols[9].first, 0U)
      , SqlPair(tableCols[10].first, 0U)
      , SqlPair(tableCols[11].first, 0U)
      , SqlPair(tableCols[12].first, 0U)
      , SqlPair(tableCols[13].first, 0U)
      , SqlPair(tableCols[14].first, 0U)
      , SqlPair(tableCols[15].first, 0U)
      , SqlPair(tableCols[16].first, data.phone2)
      , SqlPair(tableCols[17].first, data.phone3)
      , SqlPair(tableCols[18].first, data.epUeb));

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

void Address::EditEntry()
{
  auto index = m_ui->databaseView->currentIndex();
  if (index.row() == -1 || index.column() == -1)
  {
    return;
  }
  QString schl = m_ui->databaseView->model()->data(index.model()->index(index.row(), 0)).toString();

  AddressPage *page = new AddressPage(m_settings, m_query, schl, this);
  if (page->exec() == QDialog::Accepted)
  {
    AdressData data = page->data;
    std::string sql = GenerateEditCommand("ADRESSEN", "SUCHNAME", schl.toStdString()
      , SqlPair("KUNR", data.number)
      , SqlPair("ANREDE", data.salutation)
      , SqlPair("NAME", data.name)
      , SqlPair("STRASSE", data.street)
      , SqlPair("PLZ", data.plz)
      , SqlPair("ORT", data.city)
      , SqlPair("TELEFON", data.phone1)
      , SqlPair("FAX", data.fax)
      , SqlPair("TELEFON2", data.phone2)
      , SqlPair("TELEFON3", data.phone3)
      , SqlPair("EPUEB", data.epUeb));
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

void Address::DeleteEntry()
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
    m_rc = m_query.prepare("DELETE FROM ADRESSEN WHERE SUCHNAME = :ID");
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

void Address::FilterList()
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
  {}
  else
  {
    m_tableFilter = backup;
  }
  ShowDatabase();
}

void Address::PrepareDoc()
{
  auto index = m_ui->databaseView->currentIndex();
  QString id = m_ui->databaseView->model()->data(index.model()->index(index.row(), 0)).toString();
  m_rc = m_query.prepare("SELECT * FROM ADRESSEN WHERE SUCHNAME = :ID");
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

void Address::ExportToPDF()
{
  PrepareDoc();
  m_doc.print(&m_pdfPrinter);
}

void Address::PrintEntry()
{
  PrepareDoc();
  BaseTab::EmitToPrinter(m_doc);
}
