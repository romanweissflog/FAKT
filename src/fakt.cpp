#include "fakt.h"
#include "overwatch.h"
#include "export.h"
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
  m_ui->offer->SetDatabase(m_db);
  m_ui->jobsite->SetDatabase(m_db);
  m_ui->invoice->SetDatabase(m_db);

  Overwatch &instance = Overwatch::GetInstance();
  instance.AddSubject(TabName::MaterialTab, m_ui->material);
  instance.AddSubject(TabName::ServiceTab, m_ui->service);
  instance.AddSubject(TabName::AddressTab, m_ui->address);
  instance.AddSubject(TabName::OfferTab, m_ui->offer);
  instance.AddSubject(TabName::JobsiteTab, m_ui->jobsite);
  instance.AddSubject(TabName::InvoiceTab, m_ui->invoice);

  Export::Prepare();
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
  m_settings.logFile = settings.value("logFile").toString().toStdString();
  m_settings.defaultHeading = settings.value("defaultHeading").toString();

  auto &log = Log::GetLog();
  log.Initialize(m_settings.logFile);

  m_ui->service->SetSettings(&m_settings);
  m_ui->material->SetSettings(&m_settings);
  m_ui->address->SetSettings(&m_settings);
  m_ui->offer->SetSettings(&m_settings);
  m_ui->jobsite->SetSettings(&m_settings);
  m_ui->invoice->SetSettings(&m_settings);
}