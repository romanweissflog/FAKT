#include "fakt.h"
#include "overwatch.h"
#include "ui_fakt.h"

#include <iostream>

Fakt::Fakt(QWidget *parent)
  : QMainWindow(parent)
  , m_ui(new Ui::fakt)
  , m_db(QSqlDatabase::addDatabase("QSQLITE", "main"))
{
  m_ui->setupUi(this);
  m_db.setDatabaseName("fakt.db");
  m_db.open();

  m_ui->material->SetDatabase(m_db);
  m_ui->service->SetDatabase(m_db);
  m_ui->address->SetDatabase(m_db);
  m_ui->invoice->SetDatabase(m_db);

  Overwatch &instance = Overwatch::GetInstance();
  instance.AddSubject(TabNames::MaterialTab, m_ui->material);
  instance.AddSubject(TabNames::ServiceTab, m_ui->service);
  instance.AddSubject(TabNames::InvoiceTab, m_ui->invoice);
  instance.AddSubject(TabNames::MaterialTab, m_ui->material);
}

Fakt::~Fakt()
{
  if (m_db.isOpen())
  {
    m_db.close();
  }

  QSettings settings(QString::fromStdString(m_settingsPath), QSettings::Format::IniFormat);
  settings.setValue("lastOffer", QString::fromStdString(m_settings.lastOffer));
  settings.setValue("lastInvoice", QString::fromStdString(m_settings.lastInvoice));
  settings.sync();
}

void Fakt::SetSettings(std::string const &settingsPath)
{
  m_settingsPath = settingsPath;
  QSettings settings(QString::fromStdString(m_settingsPath), QSettings::Format::IniFormat);

  m_settings.euroPerMin = settings.value("euroPerMin").toDouble();
  m_settings.hourlyRate = settings.value("hourlyRate").toDouble();
  m_settings.mwst = settings.value("mwst").toDouble();
  m_settings.lastInvoice = settings.value("lastInvoice").toString().toStdString();
  m_settings.lastOffer = settings.value("lastOffer").toString().toStdString();

  m_ui->service->SetSettings(&m_settings);
  m_ui->material->SetSettings(&m_settings);
  m_ui->address->SetSettings(&m_settings);
  m_ui->invoice->SetSettings(&m_settings);
}