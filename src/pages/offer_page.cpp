#include "pages\offer_page.h"

#include "ui_general_main_page.h"

#include "QtWidgets\qlabel.h"

OfferPage::OfferPage(Settings *settings, QString const &offerNumber, QWidget *parent)
  : GeneralMainPage(settings, offerNumber, TabName::OfferTab, parent)
  , data(static_cast<OfferData*>(m_internalData.get()))
  , m_deadLineEdit(new QLineEdit(this))
  , m_deadLineErrorLabel(new QLabel(this))
{
  this->setWindowTitle("Angebot");
  m_ui->labelTypeNumber->setText("Angebotsnummber:");
  m_ui->labelTypeDate->setText("Angebots-Datum:");

  QHBoxLayout *deadLineLayout = new QHBoxLayout();
  QLabel *deadLineLabel = new QLabel("Bindefrist intern:");
  connect(m_deadLineEdit, &QLineEdit::textChanged, [this](QString txt)
  {
    data->deadLine = txt;
    if (util::IsDateValid(txt))
    {
      m_deadLineErrorLabel->setText("");
    }
    else
    {
      m_deadLineErrorLabel->setText(QString::fromStdString("Ung" + german::ue + "ltiges Datum"));
    }
  });
  m_deadLineErrorLabel->setText("");
  deadLineLayout->addWidget(deadLineLabel);
  deadLineLayout->addWidget(m_deadLineEdit);
  deadLineLayout->addWidget(m_deadLineErrorLabel);

  setTabOrder(m_ui->editHourlyRate, m_deadLineEdit);
  setTabOrder(m_deadLineEdit, m_ui->editHeading);
  setTabOrder(m_ui->editHeading, m_ui->editEnding);

  m_ui->specialDataLayout->insertLayout(3, deadLineLayout);
}

OfferPage::~OfferPage()
{}

void OfferPage::SetData(GeneralMainData *data)
{
  GeneralMainPage::SetData(data);
  OfferData *offerData = static_cast<OfferData*>(data);
  m_deadLineEdit->setText(offerData->deadLine);
}
