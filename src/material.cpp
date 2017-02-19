#include "material.h"

#include "ui_material.h"

#include "QtSql\qsqlerror.h"
#include "QtSql\qsqlquerymodel.h"
#include "QtCore\qdebug.h"
#include "QtWidgets\qlabel.h"
#include "QtWidgets\qlayout.h"
#include "QtCore\QModelIndex"

#include <iostream>

MaterialEntry::MaterialEntry(QWidget *parent)
  : Entry(parent)
{
  QHBoxLayout *layout = new QHBoxLayout();

  QLabel *artNrLabel = new QLabel("Art.Nr.:", this);
  QLineEdit *artNrEdit = new QLineEdit(this);
  connect(artNrEdit, &QLineEdit::textChanged, [&](QString txt)
  {
    data.artNumber = txt;
  });

  QLabel *descrLabel = new QLabel("Bezeichnung:", this);
  QLineEdit *descrEdit = new QLineEdit(this);
  connect(descrEdit, &QLineEdit::textChanged, [&](QString txt)
  {
    data.artDescr = txt;
  });

  QLabel *unitLabel = new QLabel("Einheit:", this);
  QLineEdit *unitEdit = new QLineEdit(this);
  connect(unitEdit, &QLineEdit::textChanged, [&](QString txt)
  {
    data.unit = txt;
  });

  QLabel *epLabel = new QLabel("EP:", this);
  QLineEdit *epEdit = new QLineEdit(this);
  connect(epEdit, &QLineEdit::textChanged, [&](QString txt)
  {
    data.ep = txt.toDouble();
  });

  m_widgets = std::vector<QWidget*>{ artNrLabel, artNrEdit, descrLabel, descrEdit,
    unitLabel, unitEdit, epLabel, epEdit };

  layout->addWidget(artNrLabel);
  layout->addWidget(artNrEdit);
  layout->addWidget(descrLabel);
  layout->addWidget(descrEdit);
  layout->addWidget(unitLabel);
  layout->addWidget(unitEdit);
  layout->addWidget(epLabel);
  layout->addWidget(epEdit);

  QVBoxLayout *mainLayout = new QVBoxLayout();
  
  mainLayout->addLayout(layout);
  mainLayout->addWidget(m_buttonBox);
  this->setLayout(mainLayout);

  this->show();
}

MaterialEntry::~MaterialEntry()
{
}

MaterialEditEntry::MaterialEditEntry(QString oldValue, QWidget *parent)
  : Entry(parent)
  , newValue(new QLineEdit(this))
{
  m_widgets.push_back(newValue);

  QHBoxLayout *layout = new QHBoxLayout();
  QLabel *oldValLabel = new QLabel("Alter Wert:");
  QLabel *oldVal = new QLabel(oldValue);
  QLabel *newValLabel = new QLabel("Neuer Wert: ");
  layout->addWidget(oldValLabel);
  layout->addWidget(oldVal);
  layout->addWidget(newValLabel);
  layout->addWidget(newValue);
  m_widgets.push_back(oldValLabel);
  m_widgets.push_back(oldVal);
  m_widgets.push_back(newValLabel);

  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->addLayout(layout);
  mainLayout->addWidget(m_buttonBox);
  this->setLayout(mainLayout);

  this->show();
}

MaterialEditEntry::~MaterialEditEntry()
{
}

MaterialDeleteEntry::MaterialDeleteEntry(QWidget *parent)
  : Entry(parent)
  , idToBeDeleted(new QLineEdit(this))
{
  m_widgets.push_back(idToBeDeleted);

  QHBoxLayout *layout = new QHBoxLayout();
  QLabel *idLabel = new QLabel("ID:");
  layout->addWidget(idLabel);
  layout->addWidget(idToBeDeleted);
  m_widgets.push_back(idLabel);

  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->addLayout(layout);
  mainLayout->addWidget(m_buttonBox);
  this->setLayout(mainLayout);

  this->show();
}

MaterialDeleteEntry::~MaterialDeleteEntry()
{
}

Material::Material(QWidget *parent)
  : QWidget(parent)
  , m_ui(new Ui::material)
  , m_db(QSqlDatabase::addDatabase("QSQLITE"))
  , m_query(m_db)
{
  m_ui->setupUi(this);
  m_db.setDatabaseName("material.db");
  m_rc = m_db.open();
  if (!m_rc)
  {
    qDebug() << m_query.lastError();
  }
  m_rc = m_query.prepare("CREATE TABLE IF NOT EXISTS Material"
    "(id INTEGER PRIMARY KEY, "
    "ArtikelNummer VARCHAR(30), "
    "Bezeichnung VARCHAR(30), "
    "Einheit VARCHAR(10), "
    "EP DOUBLE)");
  if (!m_rc)
  {
    qDebug() << m_query.lastError();
  }
  m_rc = m_query.exec();
  if (!m_rc)
  {
    qDebug() << m_query.lastError();
  }

  connect(m_ui->databaseView, &QTableView::doubleClicked, this, &Material::EditEntry);

  ShowDatabase();
}

Material::~Material()
{
  if (m_db.isOpen())
  {
    m_db.close();
  }
}

void Material::ShowDatabase()
{
  m_rc = m_db.isOpen();
  m_rc = m_query.prepare("SELECT * FROM Material");
  if (!m_rc)
  {
    qDebug() << m_query.lastError();
  }
  m_rc = m_query.exec();
  if (!m_rc)
  {
    qDebug() << m_query.lastError();
  }
  QSqlQueryModel *model = new QSqlQueryModel();
  model->setQuery(m_query);
  m_ui->databaseView->setModel(model);
}

void Material::AddEntry()
{ 
  MaterialEntry *entry = new MaterialEntry();
  if (entry->exec() == QDialog::Accepted)
  {
    MaterialEntryData data = entry->data;
    m_rc = m_query.prepare("INSERT INTO Material "
      "(ArtikelNummer, Bezeichnung, Einheit, EP)"
      "VALUES (:AN, :BE, :EI, :EP)");
    if (!m_rc)
    {
      qDebug() << m_query.lastError();
    }
    m_query.bindValue(":AN", data.artNumber);
    m_query.bindValue(":BE", data.artDescr);
    m_query.bindValue(":EI", data.unit);
    m_query.bindValue(":EP", data.ep);
    m_rc = m_query.exec();
    if (!m_rc)
    {
      qDebug() << m_query.lastError();
    }
    ShowDatabase();
  }
}

void Material::EditEntry(const QModelIndex &index)
{
  if (index.column() == 0)
  {
    return;
  }
  QString oldValue = m_ui->databaseView->model()->data(index).toString();
  QString id = m_ui->databaseView->model()->data(index.model()->index(index.row(), 0)).toString();

  MaterialEditEntry *entry = new MaterialEditEntry(oldValue, this);
  if (entry->exec() == QDialog::Accepted)
  {
    QString col;
    switch (index.column())
    {
    case 1: col = "ArtikelNummer"; break;
    case 2: col = "Bezeichnung"; break;
    case 3: col = "Einheit"; break;
    case 4: col = "EP"; break;
    default: return;
    }
    QString newValue = entry->newValue->text();
    QString stat = "UPDATE Material SET " + col + " = " + newValue + " WHERE id = " + id;
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
  MaterialDeleteEntry *entry = new MaterialDeleteEntry(this);
  if (entry->exec() == QDialog::Accepted)
  {
    QString id = entry->idToBeDeleted->text();
    m_rc = m_query.prepare("DELETE FROM Material WHERE id = :ID");
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
