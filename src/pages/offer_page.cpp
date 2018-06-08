#include "pages\offer_page.h"

#include "ui_general_main_content.h"
#include "ui_page_framework.h"

#include "QtWidgets\qlabel.h"

namespace
{
  QString GetNumber(QString const &number, Settings *settings)
  {
    if (number.size() > 0)
    {
      return number;
    }
    return QString::number(std::stoul(settings->lastOffer) + 1);
  }
}


OfferContent::OfferContent(Settings *settings, QString const &number, QWidget *parent)
  : GeneralMainContent(settings, GetNumber(number, settings), TabName::OfferTab, parent)
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

  if (number.size() == 0)
  {
    numberForSettings.emplace(QString::number(std::stoul(settings->lastOffer) + 1));
  }
}

void OfferContent::SetData(GeneralMainData *data)
{
  GeneralMainContent::SetData(data);
  OfferData *offerData = static_cast<OfferData*>(data);
  m_deadLineEdit->setText(offerData->deadLine);
}


OfferPage::OfferPage(Settings *settings,
  QString const &number,
  QWidget *parent)
  : PageFramework(parent)
  , content(new OfferContent(settings, number, this))
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

void OfferPage::HandleBeforeAccept()
{
  if (content->numberForSettings)
  {
    m_settings->lastOffer = content->numberForSettings->toStdString();
  }
}
