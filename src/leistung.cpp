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
  std::vector<std::string> tableCols
  {
    "ARTNR",
    "ARTBEZ",
    "ME",
    "EP",
    "LP",
    "MP",
    "SP",
    "BAUZEIT"
  };
  std::vector<std::string> tableViewCols
  {
    "Schl.-Nr.",
    "Bezeichnung",
    "Einheit",
    "EP",
    "Leistung",
    "Material",
    "Hilfsmat.",
    "Minuten"
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
    sql += s + ", ";
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
  for (size_t i = 0; i < constants::tableViewCols.size(); i++)
  {
    m_model->setHeaderData(i, Qt::Horizontal, QString::fromStdString(constants::tableViewCols[i]));
  }
}

void Leistung::AddEntry()
{ 
  /*LeistungEntry *entry = new LeistungEntry();
  if (entry->exec() == QDialog::Accepted)
  {
    LeistungEntryData data = entry->data;
    m_rc = m_query.prepare("INSERT INTO Leistung "
      "(SchlNr, Bezeichnung, Einheit, EP)"
      "VALUES (:AN, :BE, :EI, :EP)");
    if (!m_rc)
    {
      qDebug() << m_query.lastError();
    }
    m_query.bindValue(":AN", data.schlNumber);
    m_query.bindValue(":BE", data.descr);
    m_query.bindValue(":EI", data.unit);
    m_query.bindValue(":EP", data.ep);
    m_rc = m_query.exec();
    if (!m_rc)
    {
      qDebug() << m_query.lastError();
    }
    ShowDatabase();
  }*/
}

void Leistung::EditEntry()
{
  auto index = m_ui->databaseView->currentIndex();
  QString oldValue = m_ui->databaseView->model()->data(index).toString();
  QString schl = m_ui->databaseView->model()->data(index.model()->index(index.row(), 0)).toString();

  LeistungEditEntry *entry = new LeistungEditEntry(oldValue, this);
  if (entry->exec() == QDialog::Accepted)
  {
    QString col = QString::fromStdString(constants::tableCols[index.column()]);
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

}
