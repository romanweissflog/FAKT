#include "leistung.h"
#include "ui_basetab.h"

#include "QtSql\qsqlerror.h"
#include "QtSql\qsqlquerymodel.h"
#include "QtCore\qdebug.h"
#include "QtWidgets\qlabel.h"
#include "QtWidgets\qlayout.h"
#include "QtCore\QModelIndex"
#include "QtWidgets\qmessagebox.h"

#include <iostream>

namespace constants
{
  std::map<size_t, std::pair<std::string, std::string>> tableCols
  {
    { 0, { "ARTNR", "Schl.-Nr." }},
    { 1, {"ARTBEZ", "Bezeichnung" }},
    { 2, {"ME", "Einheit" }},
    { 3, {"EP", "EP" }},
    { 4, {"LP", "Leistung" }},
    { 5, {"MP", "Material" }},
    { 6, {"SP", "Hilfsmat." }},
    { 7, {"BAUZEIT", "Minuten" }}
  };
}


LeistungEditEntry::LeistungEditEntry(QString oldValue, QWidget *parent)
  : Entry(parent)
{
  QLineEdit *lineEdit = new QLineEdit(this);
  connect(lineEdit, &QLineEdit::textChanged, [this](QString txt)
  {
    newValue = txt;
  });
  m_widgets.push_back(lineEdit);

  QHBoxLayout *layout = new QHBoxLayout();
  QLabel *oldValLabel = new QLabel("Alter Wert:");
  QLabel *oldVal = new QLabel(oldValue);
  QLabel *newValLabel = new QLabel("Neuer Wert: ");
  layout->addWidget(oldValLabel);
  layout->addWidget(oldVal);
  layout->addWidget(newValLabel);
  layout->addWidget(lineEdit);
  m_widgets.push_back(oldValLabel);
  m_widgets.push_back(oldVal);
  m_widgets.push_back(newValLabel);

  m_layout->insertLayout(0, layout);

  this->show();
}

LeistungEditEntry::~LeistungEditEntry()
{
}


Leistung::Leistung(QWidget *parent)
  : BaseTab(parent)
{
  for (auto &&e : constants::tableCols)
  {
    m_tableFilter[e.second.first] = true;
  }
}

Leistung::~Leistung()
{
}

void Leistung::SetDatabase(QSqlDatabase &db)
{
  m_query = QSqlQuery(db);
  ShowDatabase();
}

void Leistung::ShowDatabase()
{
  std::string sql = "SELECT ";
  for (auto &&s : constants::tableCols)
  {
    if (m_tableFilter[s.second.first])
    {
      sql += s.second.first + ", ";
    }
  }
  sql = sql.substr(0, sql.size() - 2);
  sql += " FROM LEISTUNG";
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
  for (auto &&s : constants::tableCols)
  {
    if (m_tableFilter[s.second.first])
    {
      m_model->setHeaderData(idx, Qt::Horizontal, QString::fromStdString(s.second.second));
      idx++;
    }
  }
}

void Leistung::AddEntry()
{ 
}

void Leistung::EditEntry()
{
  auto index = m_ui->databaseView->currentIndex();
  QString oldValue = m_ui->databaseView->model()->data(index).toString();
  QString schl = m_ui->databaseView->model()->data(index.model()->index(index.row(), 0)).toString();

  LeistungEditEntry *entry = new LeistungEditEntry(oldValue, this);
  if (entry->exec() == QDialog::Accepted)
  {
    QString col = QString::fromStdString(constants::tableCols[index.column()].first);
    QString newValue = entry->newValue;
    QString stat = "UPDATE LEISTUNG SET " + col + " = " + newValue + " WHERE ARTNR = '" + schl + "'";
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

void Leistung::DeleteEntry()
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
    m_rc = m_query.prepare("DELETE FROM LEISTUNG WHERE ARTNR = :ID");
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

void Leistung::FilterList()
{
  std::map<std::string, std::string> mapping;
  for (auto &&s : constants::tableCols)
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
