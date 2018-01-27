#include "pages\invoice_page.h"

#include "ui_general_main_page.h"


InvoicePage::InvoicePage(Settings *settings, std::string const &invoiceNumber, TabName const &tab, QWidget *parent)
  : GeneralMainPage(settings, invoiceNumber, tab, parent)
  , data(static_cast<InvoiceData*>(m_internalData.get()))
  , m_mwstEdit(new QLineEdit(this))
  , m_deliveryEdit(new QLineEdit(this))
  , m_deliveryErrorLabel(new QLabel(this))
{
  if (tab == TabName::InvoiceTab)
  {
    this->setWindowTitle("Rechnung");
    m_ui->labelTypeNumber->setText("Rechnungsnummer:");
    m_ui->labelTypeDate->setText("Rechnungsdatum:");
  }
  else if (tab == TabName::JobsiteTab)
  {
    this->setWindowTitle("Baustelle");
    m_ui->labelTypeNumber->setText("Baustellennummer:");
    m_ui->labelTypeDate->setText("Baustelle-Datum:");
  }

  QHBoxLayout *mwstLayout = new QHBoxLayout();
  QLabel *mwstLabel = new QLabel("Mwst. (%):");
  connect(m_mwstEdit, &QLineEdit::textChanged, [this](QString txt)
  {
    data->mwst = txt.toDouble();
  });
  mwstLayout->addWidget(mwstLabel);
  mwstLayout->addWidget(m_mwstEdit);
  m_ui->specialDataLayout->insertLayout(0, mwstLayout);

  QHBoxLayout *deliveryLayout = new QHBoxLayout();
  QLabel *deliveryLabel = new QLabel("Lieferg. v.:");
  connect(m_deliveryEdit, &QLineEdit::textChanged, [this](QString txt)
  {
    data->deliveryDate = txt;
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

  setTabOrder(m_ui->editSubject, m_mwstEdit);
  setTabOrder(m_mwstEdit, m_ui->editSkonto);
  setTabOrder(m_ui->editSkonto, m_ui->editPayNormal);
  setTabOrder(m_ui->editPayNormal, m_deliveryEdit);
  setTabOrder(m_deliveryEdit, m_ui->editHourlyRate);
  setTabOrder(m_ui->editHourlyRate, m_ui->editHeading);
  setTabOrder(m_ui->editHeading, m_ui->editEnding);
  setTabOrder(m_ui->editEnding, m_ui->buttonHeading);
  setTabOrder(m_ui->buttonHeading, m_ui->buttonEnding);
}

InvoicePage::~InvoicePage()
{}

void InvoicePage::SetData(GeneralMainData *data)
{
  GeneralMainPage::SetData(data);
  InvoiceData *invoiceData = static_cast<InvoiceData*>(data);
  m_mwstEdit->setText(QString::number(invoiceData->mwst));
  m_deliveryEdit->setText(invoiceData->deliveryDate);
}
