#include "material.h"
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
    { 0, { "ARTNR", "Schl.-Nr." } },
    { 1, { "ARTBEZ", "Bezeichnung" } },
    { 2, { "ME", "Einheit" } },
    { 3, { "NETTO", "Netto" } },
    { 4, { "BRUTTO", "Brutto" } },
    { 5, { "EKP", "EKP" } },
    { 6, { "EP", "Verarb.-Preis" } },
    { 7, { "LIEFERER", "Lieferant"} },
    { 8, { "BAUZEIT", "Minuten" } },
    { 9, { "BESTAND", "Bestand" } }
  };
}


Material::Material(QWidget *parent)
  : BaseTab("Material", PrintType::PrintTypeMaterial, parent)
{
  for (auto &&e : tableCols)
  {
    m_tableFilter[e.second.first] = true;
  }
}

Material::~Material()
{
}

void Material::SetDatabase(QSqlDatabase &db)
{
  m_query = QSqlQuery(db);
  ShowDatabase();
}

void Material::ShowDatabase()
{
  std::string sql = "SELECT ";
  for (auto &&s : tableCols)
  {
    if (m_tableFilter[s.second.first])
    {
      sql += s.second.first + ", ";
    }
  }
  sql = sql.substr(0, sql.size() - 2);
  sql += " FROM MATERIAL";
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
  }
}

void Material::AddEntry()
{
  MaterialPage *page = new MaterialPage(m_settings, m_query, this);
  if (page->exec() == QDialog::Accepted)
  {
    auto &data = page->data;
    std::string sql = GenerateInsertCommand("LEISTUNG"
      , SqlPair(tableCols[0].first, data.key)
      , SqlPair(tableCols[1].first, data.description)
      , SqlPair(tableCols[2].first, data.unit)
      , SqlPair(tableCols[3].first, data.netto)
      , SqlPair(tableCols[4].first, data.brutto)
      , SqlPair(tableCols[5].first, data.ekp)
      , SqlPair(tableCols[6].first, data.ep)
      , SqlPair(tableCols[7].first, data.supplier)
      , SqlPair(tableCols[8].first, data.minutes)
      , SqlPair(tableCols[9].first, data.stockSize));
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

void Material::EditEntry()
{
  auto index = m_ui->databaseView->currentIndex();
  if (index.row() == -1 || index.column() == -1)
  {
    return;
  }
  QString oldValue = m_ui->databaseView->model()->data(index).toString();
  QString schl = m_ui->databaseView->model()->data(index.model()->index(index.row(), 0)).toString();

  EditOneEntry *entry = new EditOneEntry(oldValue, this);
  if (entry->exec() == QDialog::Accepted)
  {
    QString col = QString::fromStdString(tableCols[index.column()].first);
    QString newValue = entry->newValue;
    QString stat = "UPDATE MATERIAL SET " + col + " = " + newValue + " WHERE ARTNR = '" + schl + "'";
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

void Material::DeleteEntry()
{
  auto index = m_ui->databaseView->currentIndex();
  if (index.row() == -1 || index.column() == -1)
  {
    return;
  }
  QMessageBox *question = new QMessageBox(this);
  question->setWindowTitle("WARNUNG");
  question->setText("Wollen sie den Eintrag entfernen?");
  question->setStandardButtons(QMessageBox::Yes);
  question->addButton(QMessageBox::No);
  question->setDefaultButton(QMessageBox::No);
  if (question->exec() == QMessageBox::Yes)
  {
    QString id = m_ui->databaseView->model()->data(index.model()->index(index.row(), 0)).toString();
    m_rc = m_query.prepare("DELETE FROM MATERIAL WHERE ARTNR = :ID");
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

void Material::FilterList()
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

void Material::PrepareDoc()
{
  auto index = m_ui->databaseView->currentIndex();
  QString id = m_ui->databaseView->model()->data(index.model()->index(index.row(), 0)).toString();
  m_rc = m_query.prepare("SELECT * FROM MATERIAL WHERE ARTNR = :ID");
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

void Material::ExportToPDF()
{
  PrepareDoc();
  m_doc.print(&m_pdfPrinter);
}

void Material::PrintEntry()
{
  PrepareDoc();
  BaseTab::EmitToPrinter(m_doc);
}

Data* Material::GetData(std::string const &artNr)
{
  MaterialData *data = new MaterialData;
  m_rc = m_query.prepare("SELECT * FROM MATERIAL WHERE ARTNR = :ID");
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

  data->key = m_query.value(1).toString();
  data->description = m_query.value(2).toString();
  data->unit = m_query.value(3).toString();
  data->ekp = m_query.value(4).toDouble();
  data->netto = m_query.value(5).toDouble();
  data->stockSize = m_query.value(6).toDouble();
  data->supplier = m_query.value(7).toString();
  data->minutes = m_query.value(8).toDouble();
  data->brutto = m_query.value(9).toDouble();
  data->ep = m_query.value(10).toDouble();
  return data;
}

std::vector<QString> Material::GetArtNumbers()
{
  std::vector<QString> list;
  MaterialData *data = new MaterialData;
  m_rc = m_query.exec("SELECT ARTNR FROM MATERIAL");
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