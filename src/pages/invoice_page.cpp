#include "pages\invoice_page.h"

#include "ui_general_main_content.h"
#include "ui_page_framework.h"

namespace
{
  QString GetNumber(TabName const &tab, QString const &number, Settings *settings)
  {
    if (number.size() > 0)
    {
      return number;
    }
    if (tab == TabName::InvoiceTab)
    {
      return QString::number(std::stoul(settings->lastInvoice) + 1);
    }
    else
    {
      return QString::number(std::stoul(settings->lastJobsite) + 1);
    }
  }
}


InvoiceContent::InvoiceContent(Settings *settings, QString const &number, TabName const &tab, QWidget *parent)
  : GeneralMainContent(settings, GetNumber(tab, number, settings), tab, parent)
  , m_mwstEdit(new QLineEdit(this))
  , m_deliveryEdit(new QLineEdit(this))
  , m_deliveryErrorLabel(new QLabel(this))
{
  if (tab == TabName::InvoiceTab)
  {
    this->setWindowTitle("Rechnung");
    m_ui->labelTypeNumber->setText("Rechnungsnummer:");
    m_ui->labelTypeDate->setText("Rechnungsdatum:");

    if (number.size() == 0)
    {
      numberForSettings.emplace(QString::number(std::stoul(settings->lastInvoice) + 1));
    }
  }
  else if (tab == TabName::JobsiteTab)
  {
    this->setWindowTitle("Baustelle");
    m_ui->labelTypeNumber->setText("Baustellennummer:");
    m_ui->labelTypeDate->setText("Baustelle-Datum:");

    if (number.size() == 0)
    {
      numberForSettings.emplace(QString::number(std::stoul(settings->lastJobsite) + 1));
    }
  }

  QHBoxLayout *mwstLayout = new QHBoxLayout();
  QLabel *mwstLabel = new QLabel("Mwst. (%):");
  connect(m_mwstEdit, &QLineEdit::textChanged, [this](QString txt)
  {
    QLocale l(QLocale::German);
    data["MWSTSATZ"].entry = l.toDouble(txt);
  });
  mwstLayout->addWidget(mwstLabel);
  mwstLayout->addWidget(m_mwstEdit);
  m_ui->specialDataLayout->insertLayout(0, mwstLayout);
  m_mwstEdit->setText(QString::number(settings->mwst));

  QHBoxLayout *deliveryLayout = new QHBoxLayout();
  QLabel *deliveryLabel = new QLabel("Lieferg. v.:");
  m_deliveryErrorLabel->setStyleSheet("color: red");
  connect(m_deliveryEdit, &QLineEdit::textChanged, [this](QString txt)
  {
    QLocale l(QLocale::German);
    data["LIEFDAT"].entry = l.toDouble(txt);
    if (util::IsDateValid(txt))
    {
      m_deliveryErrorLabel->setText("");
    }
    else
    {
      m_deliveryErrorLabel->setText(QString::fromStdString("Ung" + german::ue + "ltiges Datum"));
    }
  });
  deliveryLayout->addWidget(deliveryLabel);
  deliveryLayout->addWidget(m_deliveryEdit);
  deliveryLayout->addWidget(m_deliveryErrorLabel);
  m_ui->specialDataLayout->insertLayout(3, deliveryLayout);
  m_deliveryEdit->setText(m_ui->editDate->text());

  setTabOrder(m_ui->editSubject, m_mwstEdit);
  setTabOrder(m_mwstEdit, m_ui->editSkonto);
  setTabOrder(m_ui->editSkonto, m_ui->editPayNormal);
  setTabOrder(m_ui->editPayNormal, m_ui->editPaySkonto);
  setTabOrder(m_ui->editPaySkonto, m_deliveryEdit);
  setTabOrder(m_deliveryEdit, m_ui->editHourlyRate);
  setTabOrder(m_ui->editHourlyRate, m_ui->editHeading);
  setTabOrder(m_ui->editHeading, m_ui->editEnding);
}

void InvoiceContent::SetData(DatabaseData const &data)
{
  GeneralMainContent::SetData(data);
  m_mwstEdit->setText(QString::number(data.GetDouble("MWSTSATZ")));
  m_deliveryEdit->setText(data.GetString("LIEFDAT"));
}


InvoicePage::InvoicePage(Settings *settings,
  QString const &number,
  TabName const &childType,
  QWidget *parent)
  : PageFramework(parent)
  , content(new InvoiceContent(settings, number, childType, this))
  , m_settings(settings)
  , m_childType(childType)
{
  m_ui->mainLayout->replaceWidget(m_ui->content, content);

  content->setFocus();
  content->SetFocusToFirst();

  connect(content, &GeneralMainContent::AddPage, [this]()
  {
    emit AddExtraPage(content->importPage, "Import Adresse");
  });
  connect(content, &GeneralMainContent::ClosePage, [this]()
  {
    emit CloseExtraPage("Import Adresse");
    content->setFocus();
  });
}

void InvoicePage::HandleBeforeAccept()
{
  if (content->numberForSettings)
  {
    if (m_childType == TabName::InvoiceTab)
    {
      m_settings->lastInvoice = content->numberForSettings->toStdString();
    }
    else
    {
      m_settings->lastJobsite = content->numberForSettings->toStdString();
    }
  }
}
