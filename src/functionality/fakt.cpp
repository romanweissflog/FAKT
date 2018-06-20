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
#include "pages\payment_page.h"
#include "pages\page_framework.h"
#include "ui_fakt.h"

#include "QtWidgets\qshortcut.h"
#include "QtWidgets\qpushbutton.h"

#include <iostream>

namespace
{
  std::vector<QString> tabNames
  {
    "Material", "Leistungen", "Adressen", "Angebote", "Baustellen", "Rechnungen"
  };
}


Fakt::Fakt(QSplashScreen *splashScreen, QWidget *parent)
  : QMainWindow(parent)
  , m_ui(new Ui::fakt)
  , m_db(QSqlDatabase::addDatabase("QSQLITE", "main"))
{
  m_ui->setupUi(this);
  setWindowTitle("FAKT");

  connect(this, &Fakt::SetMessage, [splashScreen](QString const &txt)
  {
    splashScreen->showMessage(txt, Qt::AlignBottom, Qt::white);
  });
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

  emit SetMessage("Preparing Material...");
  m_tabs.push_back(new Material(this));
  emit SetMessage("Preparing Service...");
  m_tabs.push_back(new Service(this));
  emit SetMessage("Preparing Address...");
  m_tabs.push_back(new Address(this));
  emit SetMessage("Preparing Offer...");
  m_tabs.push_back(new Offer(this));
  emit SetMessage("Preparing Jobsite...");
  m_tabs.push_back(new Jobsite(this));
  emit SetMessage("Preparing Invoice...");
  m_tabs.push_back(new Invoice(this));

  emit SetMessage("Preparing Connections...");
  for (auto &&t : m_tabs)
  {
    t->hide();
    connect(t, static_cast<void (BaseTab::*)(QWidget*, QString const &)>(&BaseTab::AddSubtab),
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

  instance.SetDatabase(m_db);

  for (auto &&t : m_tabs)
  {
    t->SetSettings(&m_settings);
    t->SetDatabase(m_db);
  }

  connect(m_ui->main, &MainTab::AddTab, this, &Fakt::AddTab);
  emit SetMessage("Done");
}

void Fakt::SetSettings(std::string const &settingsPath)
{
  emit SetMessage("Loading Settings...");
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
  m_settings.defaultHeadline = settings.value("defaultHeadline").toString();
  m_settings.defaultOfferEndline = settings.value("defaultOfferEndline").toString();
  m_settings.defaultInvoiceEndline = settings.value("defaultInvoiceEndline").toString();
  m_settings.skontoTextShort = settings.value("skontoTextShort").toString();
  m_settings.skontoTextLong = settings.value("skontoTextLong").toString();
  m_settings.discountText = settings.value("discountText").toString();

  settings.beginGroup("Constants");
  m_settings.constants.rowOffset = settings.value("rowOffset").toInt();
  settings.endGroup();

  auto &log = Log::GetLog();
  log.Initialize(m_settings.logFile);
  connect(&log, &Log::ShowMessage, [this](QString txt)
  {
    m_ui->errorMessage->setText(txt);
  });
  connect(m_ui->clearError, &QPushButton::clicked, [this]()
  {
    m_ui->errorMessage->setText("");
  });
}

void Fakt::AddTab(int idx)
{
  if (m_openTabs.count(tabNames[idx]) == 0)
  {
    m_openTabs[tabNames[idx]] = m_ui->tabWidget->count();
    m_ui->tabWidget->addTab(m_tabs[idx], tabNames[idx]);
  }
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
