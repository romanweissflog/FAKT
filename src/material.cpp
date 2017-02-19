#include "material.h"

#include "ui_material.h"

#include "QtSql\qsqlerror.h"
#include "QtSql\qsqlquerymodel.h"
#include "QtCore\qdebug.h"
#include "QtWidgets\qlabel.h"
#include "QtWidgets\qlayout.h"
#include "QtWidgets\qlineedit.h"

#include <iostream>

MaterialEntry::MaterialEntry(QWidget *parent)
  : QDialog(parent)
  , m_buttonBox(new QDialogButtonBox(this))
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

  QPushButton *cancelButton = new QPushButton("Cancel", this);
  m_buttonBox->addButton(cancelButton, QDialogButtonBox::RejectRole);
  connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

  QPushButton *okButton = new QPushButton("OK", this);
  m_buttonBox->addButton(okButton, QDialogButtonBox::AcceptRole);
  connect(okButton, &QPushButton::clicked, this, &QDialog::accept);

  m_widgets.push_back(cancelButton);
  m_widgets.push_back(okButton);
  
  mainLayout->addLayout(layout);
  mainLayout->addWidget(m_buttonBox);
  this->setLayout(mainLayout);

  this->show();
}

MaterialEntry::~MaterialEntry()
{
  for (auto w : m_widgets)
  {
    delete w;
  }
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

void Material::EditEntry(QModelIndex index)
{

}