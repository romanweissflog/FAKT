#include "adresse.h"
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
    { 6,  { "STRASSE", "Stra�e" } },
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
    { 18, { "FIBUKONTO", "Fibu" } },
    { 19, { "ANSPRECH", "Ansprache" } }
  };
}


Adresse::Adresse(QWidget *parent)
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

Adresse::~Adresse()
{
}

void Adresse::SetDatabase(QSqlDatabase &db)
{
  m_query = QSqlQuery(db);
  ShowDatabase();
}

void Adresse::ShowDatabase()
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

void Adresse::AddEntry()
{ 
  LeistungPage *page = new LeistungPage(m_settings, m_query, this);
  if (page->exec() == QDialog::Accepted)
  {
    auto &data = page->data;
    std::string sql = "INSERT INTO ADRESSEN (";
    for (auto &&s : tableCols)
    {
      sql += s.second.first + ", ";
    }
    sql = sql.substr(0, sql.size() - 2);
    sql += ") VALUES ('" + data.key.toStdString() + "', '" +
      data.description.toStdString() + "', '" +
      data.unit.toStdString() + "', " +
      std::to_string(data.ep) + ", " +
      std::to_string(data.service) + ", " +
      std::to_string(data.material) + ", " +
      std::to_string(data.helperMaterial) + ", " +
      std::to_string(data.minutes) + ", " +
      std::to_string(data.ekp) + ")";
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

void Adresse::EditEntry()
{
  auto index = m_ui->databaseView->currentIndex();
  QString oldValue = m_ui->databaseView->model()->data(index).toString();
  QString schl = m_ui->databaseView->model()->data(index.model()->index(index.row(), 0)).toString();

  EditOneEntry *entry = new EditOneEntry(oldValue, this);
  if (entry->exec() == QDialog::Accepted)
  {
    QString col = QString::fromStdString(tableCols[index.column()].first);
    QString newValue = entry->newValue;
    QString stat = "UPDATE ADRESSEN SET " + col + " = " + newValue + " WHERE SUCHNAME = '" + schl + "'";
    m_rc = m_query.prepare(stat);
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

void Adresse::DeleteEntry()
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
    m_rc = m_query.prepare("DELETE FROM ADRESSEN WHERE ARTNR = :ID");
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

void Adresse::FilterList()
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

void Adresse::PrepareDoc()
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

void Adresse::ExportToPDF()
{
  PrepareDoc();
  m_doc.print(&m_pdfPrinter);
}

void Adresse::PrintEntry()
{
  PrepareDoc();
  BaseTab::EmitToPrinter(m_doc);
}