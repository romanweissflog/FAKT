#include "material.h"

#include "ui_material.h"

#include "QtSql\qsqlerror.h"
#include "QtSql\qsqlquerymodel.h"
#include "QtCore\qdebug.h"

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