#include "address.h"
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
    { 8,  { "FAX", "Fax" } }
  };
}


Address::Address(QWidget *parent)
  : BaseTab("Address", PrintType::PrintTypeAddress, parent)
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
      m_model->setHeaderData((int)idx, Qt::Horizontal, QString::fromStdString(s.second.second));
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
      , SqlPair(tableCols[1].first, data.phone)
      , SqlPair(tableCols[2].first, data.number)
      , SqlPair(tableCols[3].first, data.name)
      , SqlPair(tableCols[4].first, data.plz)
      , SqlPair(tableCols[5].first, data.city)
      , SqlPair(tableCols[6].first, data.street)
      , SqlPair(tableCols[7].first, data.salutation)
      , SqlPair(tableCols[8].first, data.fax));

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
    AddressData data = page->data;
    std::string sql = GenerateEditCommand("ADRESSEN", "SUCHNAME", schl.toStdString()
      , SqlPair("KUNR", data.number)
      , SqlPair("ANREDE", data.salutation)
      , SqlPair("NAME", data.name)
      , SqlPair("STRASSE", data.street)
      , SqlPair("PLZ", data.plz)
      , SqlPair("ORT", data.city)
      , SqlPair("TELEFON", data.phone)
      , SqlPair("FAX", data.fax));
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
    html += "<th>" + m_query.value((int)i).toString().toStdString() + "</td>";
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

Data* Address::GetData(std::string const &customer)
{
  AddressData *data = new AddressData;
  m_rc = m_query.prepare("SELECT * FROM ADRESSEN WHERE SUCHNAME = :ID");
  if (!m_rc)
  {
    qDebug() << m_query.lastError();
  }
  m_query.bindValue(":ID", QString::fromStdString(customer));
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
  
  data->key = m_query.value(1).toString();
  data->number = m_query.value(2).toUInt();
  data->salutation = m_query.value(3).toString();
  data->name = m_query.value(4).toString();
  data->street = m_query.value(5).toString();
  data->plz = m_query.value(6).toString();
  data->city = m_query.value(7).toString();
  data->phone = m_query.value(8).toString();
  data->fax = m_query.value(9).toString();
  return data;
}

std::vector<QString> Address::GetArtNumbers()
{
  std::vector<QString> list;
  m_rc = m_query.exec("SELECT SUCHNAME FROM ADRESSEN");
  if (!m_rc)
  {
    qDebug() << m_query.lastError();
  }
  while (m_query.next())
  {
    list.push_back(m_query.value(0).toString());
  }
  return list;
}
