#include "pages\general_print_page.h"
#include "functionality\log.h"
#include "functionality\export.h"
#include "functionality\overwatch.h"

#include "ui_general_print_page.h"

#include "QtWidgets\qpushbutton.h"
#include "QtWidgets\qshortcut.h"

GeneralPrintPage::GeneralPrintPage(TabName const &parentTab, GeneralMainData const &data, uint16_t &subMask, QWidget *parent)
  : QDialog(parent)
  , m_ui(new Ui::generalPrintPage)
  , m_logId(Log::GetLog().RegisterInstance("GeneralPrintPage"))
  , m_subMask(subMask)
{
  m_ui->setupUi(this);
  this->setObjectName("printPage");
  this->setAttribute(Qt::WA_DeleteOnClose);

  // prepare static gui data
  m_ui->date->setText(data.date);
  m_ui->number->setText(data.number);
  m_ui->salutation->setText(data.salutation);
  m_ui->name->setText(data.name);
  m_ui->street->setText(data.street);
  m_ui->place->setText(data.place);

  connect(m_ui->cancel, &QPushButton::clicked, [this]() { emit Close(); });

  connect(m_ui->positionsShort, &QPushButton::clicked, [this]() { m_subMask = (printmask::Short | printmask::Position); accept(); });
  connect(m_ui->positionsLong, &QPushButton::clicked, [this]() { m_subMask = (printmask::Long | printmask::Position); accept(); });
  connect(m_ui->groupsShort, &QPushButton::clicked, [this]() { m_subMask = (printmask::Short | printmask::Groups); accept(); });
  connect(m_ui->groupsLong, &QPushButton::clicked, [this]() { m_subMask = (printmask::Long | printmask::Groups); accept(); });
  connect(m_ui->allShort, &QPushButton::clicked, [this]() { m_subMask = (printmask::Short | printmask::All); accept(); });
  connect(m_ui->allLong, &QPushButton::clicked, [this]() { m_subMask = (printmask::Long | printmask::All); accept(); });
  m_ui->positionsShort->installEventFilter(Overwatch::GetInstance().GetEventLogger());
  m_ui->positionsLong->installEventFilter(Overwatch::GetInstance().GetEventLogger());
  m_ui->groupsShort->installEventFilter(Overwatch::GetInstance().GetEventLogger());
  m_ui->groupsLong->installEventFilter(Overwatch::GetInstance().GetEventLogger());
  m_ui->allShort->installEventFilter(Overwatch::GetInstance().GetEventLogger());
  m_ui->allLong->installEventFilter(Overwatch::GetInstance().GetEventLogger());

  SHORTCUTSIGNAL(key1, Key_1, m_subMask = (printmask::Short | printmask::Position); accept())
  SHORTCUTSIGNAL(key2, Key_2, m_subMask = (printmask::Long | printmask::Position); accept())
  if (parentTab == TabName::JobsiteTab)
  {
    m_ui->groupsShort->setEnabled(false);
    m_ui->groupsLong->setEnabled(false);
    m_ui->allShort->setEnabled(false);
    m_ui->allLong->setEnabled(false);
  }
  else
  {
    SHORTCUTSIGNAL(key3, Key_3, m_subMask = (printmask::Short | printmask::Groups); accept())
    SHORTCUTSIGNAL(key4, Key_4, m_subMask = (printmask::Long | printmask::Groups); accept())
    SHORTCUTSIGNAL(key5, Key_5, m_subMask = (printmask::Short | printmask::All); accept())
    SHORTCUTSIGNAL(key6, Key_6, m_subMask = (printmask::Long | printmask::All); accept())
  }

  switch (parentTab)
  {
  case TabName::InvoiceTab: SetInvoiceData(); break;
  case TabName::JobsiteTab: SetJobsiteData(); break;
  case TabName::OfferTab: SetOfferData(); break;
  default:
    Log::GetLog().Write(LogType::LogTypeError, m_logId, "invalid tab for printing");
    return;
  }

  this->installEventFilter(Overwatch::GetInstance().GetEventLogger());
}

void GeneralPrintPage::keyPressEvent(QKeyEvent *e)
{
  if (e->key() != Qt::Key_Escape)
  {
    QDialog::keyPressEvent(e);
  }
  else
  {
    emit Close();
  }
}

void GeneralPrintPage::SetInvoiceData()
{
  m_ui->typeNumber->setText("Rechnungs-Nummer:");
  m_ui->typeDate->setText("Rechnungs-Datum:");
}

void GeneralPrintPage::SetJobsiteData()
{
  m_ui->typeNumber->setText("Baustellen-Nummer:");
  m_ui->typeDate->setText("Baustellen-Datum:");
}

void GeneralPrintPage::SetOfferData()
{
  m_ui->typeNumber->setText("Angebots-Nummer:");
  m_ui->typeDate->setText("Angebots-Datum:");
}
