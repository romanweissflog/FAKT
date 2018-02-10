#include "functionality\fakt.h"
#include "functionality\overwatch.h"
#include "functionality\export.h"
#include "tabs\material.h"
#include "tabs\service.h"
#include "tabs\address.h"
#include "tabs\offer.h"
#include "tabs\jobsite.h"
#include "tabs\invoice.h"
#include "tabs\payment.h"
#include "pages\single_entry.h"
#include "pages\general_main_page.h"
#include "pages\payment_page.h"
#include "ui_fakt.h"

#include "QtWidgets\qshortcut.h"

#include <iostream>

namespace
{
  std::vector<QString> tabNames
  {
    "Material", "Leistungen", "Adressen", "Angebote", "Baustellen", "Rechnungen"
  };
}


Fakt::Fakt(QWidget *parent)
  : QMainWindow(parent)
  , m_ui(new Ui::fakt)
  , m_db(QSqlDatabase::addDatabase("QSQLITE", "main"))
{
  m_ui->setupUi(this);
  setWindowTitle("FAKT");
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
  settings.setValue("lastJobsite", QString::fromStdString(m_settings.lastJobsite));
  settings.setValue("lastCustomer", QString::fromStdString(m_settings.lastCustomer));
  settings.sync();
}

void Fakt::Init()
{

  QFont font;
  font.setPointSize(10);
  font.setStyleHint(QFont::Monospace);
  QApplication::setFont(font);

  m_db.setDatabaseName("fakt.db");
  m_db.open();

  m_tabs.push_back(new Material(this));
  m_tabs.push_back(new Service(this));
  m_tabs.push_back(new Address(this));
  m_tabs.push_back(new Offer(this));
  m_tabs.push_back(new Jobsite(this));
  m_tabs.push_back(new Invoice(this));

  for (auto &&t : m_tabs)
  {
    t->hide();
    t->SetDatabase(m_db);
    connect(t, static_cast<void (BaseTab::*)(QWidget*, QString const &)>(&BaseTab::AddSubtab),
      this, static_cast<void (Fakt::*)(QWidget*, QString const &)>(&Fakt::AddSubtab));
    connect(t, static_cast<void (BaseTab::*)(GeneralMainPage*, QString const &)>(&BaseTab::AddSubtab),
      this, static_cast<void (Fakt::*)(QWidget*, QString const &)>(&Fakt::AddSubtab));
    connect(t, static_cast<void (BaseTab::*)(SingleEntry*, QString const &)>(&BaseTab::AddSubtab),
      this, static_cast<void (Fakt::*)(SingleEntry*, QString const &)>(&Fakt::AddSubtab));
    connect(t, static_cast<void (BaseTab::*)(Payment*, QString const &)>(&BaseTab::AddSubtab),
      this, static_cast<void (Fakt::*)(Payment*, QString const &)>(&Fakt::AddSubtab));
    connect(t, &BaseTab::CloseTab, this, &Fakt::RemoveTab);
  }

  Overwatch &instance = Overwatch::GetInstance();
  instance.AddSubject(TabName::MaterialTab, m_tabs[0]);
  instance.AddSubject(TabName::ServiceTab, m_tabs[1]);
  instance.AddSubject(TabName::AddressTab, m_tabs[2]);
  instance.AddSubject(TabName::OfferTab, m_tabs[3]);
  instance.AddSubject(TabName::JobsiteTab, m_tabs[4]);
  instance.AddSubject(TabName::InvoiceTab, m_tabs[5]);

  connect(m_ui->main, &MainTab::AddTab, this, &Fakt::AddTab);
}

void Fakt::SetSettings(std::string const &settingsPath)
{
  m_settingsPath = settingsPath;
  QSettings settings(QString::fromStdString(m_settingsPath), QSettings::Format::IniFormat);
  settings.setIniCodec("UTF-8");

  m_backup(&settings);

  m_settings.hourlyRate = settings.value("hourlyRate").toDouble();
  m_settings.mwst = settings.value("mwst").toDouble();
  m_settings.lastInvoice = settings.value("lastInvoice").toString().toStdString();
  m_settings.lastOffer = settings.value("lastOffer").toString().toStdString();
  m_settings.lastJobsite = settings.value("lastJobsite").toString().toStdString();
  m_settings.lastCustomer = settings.value("lastCustomer").toString().toStdString();
  m_settings.logFile = settings.value("logFile").toString().toStdString();
  m_settings.logoFile = settings.value("logoFile").toString().toStdString();
  m_settings.defaultHeadline = settings.value("defaultHeadline").toString().toStdString();
  m_settings.defaultEndline = settings.value("defaultEndline").toString().toStdString();

  auto &log = Log::GetLog();
  log.Initialize(m_settings.logFile);

  for (auto &&t : m_tabs)
  {
    t->SetSettings(&m_settings);
  }
}

void Fakt::AddTab(int idx)
{
  if (m_openTabs.count(tabNames[idx]) == 0)
  {
    m_openTabs[tabNames[idx]] = m_ui->tabWidget->count();
  }
  m_ui->tabWidget->addTab(m_tabs[idx], tabNames[idx]);
  m_ui->tabWidget->setCurrentIndex(m_openTabs[tabNames[idx]]);
}

void Fakt::AddSubtab(QWidget *tab, QString const &name)
{
  if (m_openTabs.count(name) == 0)
  {
    m_openTabs[name] = m_ui->tabWidget->count();
  }
  m_ui->tabWidget->addTab(tab, name);
  m_ui->tabWidget->setCurrentIndex(m_openTabs[name]);
  tab->setFocus();

  std::string stdName = name.toStdString();
  QString type = QString::fromStdString(stdName.substr(0, stdName.find_first_of(":")));
  if (m_openTabs.count(type) == 0)
  {
    return;
  }
  m_ui->tabWidget->setTabEnabled(m_openTabs[type], false);
  QString subType = QString::fromStdString(stdName.substr(0, stdName.find_last_of(":")));
  if (m_openTabs.count(subType) == 0)
  {
    return;
  }
  m_ui->tabWidget->setTabEnabled(m_openTabs[subType], false);
}

void Fakt::AddSubtab(SingleEntry *tab, QString const &name)
{
  // general page
  connect(tab, static_cast<void (BaseTab::*)(GeneralPage*, QString const &)>(&BaseTab::AddSubtab),
    this, static_cast<void (Fakt::*)(QWidget*, QString const &)>(&Fakt::AddSubtab));

  // general main page
  connect(tab, static_cast<void (BaseTab::*)(GeneralMainPage*, QString const &)>(&BaseTab::AddSubtab),
    this, static_cast<void (Fakt::*)(QWidget*, QString const &)>(&Fakt::AddSubtab));

  // widgets
  connect(tab, static_cast<void (BaseTab::*)(QWidget*, QString const &)>(&BaseTab::AddSubtab),
    this, static_cast<void (Fakt::*)(QWidget*, QString const &)>(&Fakt::AddSubtab));

  connect(tab, &SingleEntry::CloseTab, this, &Fakt::RemoveTab);

  AddSubtab(qobject_cast<QWidget*>(tab), name);
}

void Fakt::AddSubtab(Payment *tab, QString const &name)
{
  connect(tab, &Payment::CloseTab, this, &Fakt::RemoveTab);

  // payment page
  connect(tab, static_cast<void (BaseTab::*)(QWidget*, QString const &)>(&BaseTab::AddSubtab),
    this, static_cast<void (Fakt::*)(QWidget*, QString const &)>(&Fakt::AddSubtab));

  //connect(tab, &BaseTab::CloseTab, this, &Fakt::RemoveTab);

  AddSubtab(qobject_cast<QWidget*>(tab), name);
  tab->SetDatabase(m_db);
}

void Fakt::RemoveTab(QString const &tab)
{
  if (m_openTabs.count(tab) == 0)
  {
    return;
  }
  auto idx = m_openTabs[tab];
  m_ui->tabWidget->removeTab(idx);
  m_openTabs.erase(tab);
  for (auto &&t : m_openTabs)
  {
    if (t.second > idx)
    {
      --t.second;
    }
  }

  std::string stdName = tab.toStdString();
  if (stdName.find(":") == std::string::npos)
  {
    return;
  }
  auto pos1 = stdName.find_first_of(":");
  auto pos2 = stdName.find_last_of(":");
  QString type = QString::fromStdString(stdName.substr(0, pos1));
  if (m_openTabs.count(type) == 0)
  {
    return;
  }
  QString subType = QString::fromStdString(stdName.substr(0, pos2));
  if (m_openTabs.count(subType) == 0)
  {
    return;
  }
  if (pos2 != pos1)
  {
    m_ui->tabWidget->setTabEnabled(m_openTabs[subType], true);
  }
  else
  {
    m_ui->tabWidget->setTabEnabled(m_openTabs[type], true);
  }
}
