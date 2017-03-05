#include "fakt.h"
#include "ui_fakt.h"

#include "QtCore\qsettings.h"

Fakt::Fakt(QWidget *parent)
  : QMainWindow(parent)
  , m_ui(new Ui::fakt)
  , m_db(QSqlDatabase::addDatabase("QSQLITE"))
{
  m_ui->setupUi(this);
  m_db.setDatabaseName("fakt.db");
  m_db.open();

  m_ui->material->SetDatabase(m_db);
  m_ui->service->SetDatabase(m_db);
  m_ui->adresse->SetDatabase(m_db);
  m_ui->rechnung->SetDatabase(m_db);

  SetSettings();
}

Fakt::~Fakt()
{
  if (m_db.isOpen())
  {
    m_db.close();
  }
}

void Fakt::SetSettings()
{
  QSettings settings(QApplication::applicationDirPath() + "/settings.ini", 
    QSettings::Format::IniFormat);

  m_ui->service->SetSettings(&m_settings);
  m_ui->material->SetSettings(&m_settings);
  m_ui->adresse->SetSettings(&m_settings);
}