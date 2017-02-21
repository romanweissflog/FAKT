#include "fakt.h"
#include "ui_fakt.h"

Fakt::Fakt(QWidget *parent)
  : QMainWindow(parent)
  , m_ui(new Ui::fakt)
  , m_db(QSqlDatabase::addDatabase("QSQLITE"))
{
  m_ui->setupUi(this);
  m_db.setDatabaseName("fakt.db");
  m_db.open();

  m_ui->material->SetDatabase(m_db);
  m_ui->leistung->SetDatabase(m_db);
}

Fakt::~Fakt()
{
  if (m_db.isOpen())
  {
    m_db.close();
  }
}