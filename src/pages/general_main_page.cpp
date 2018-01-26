#include "pages\general_main_page.h"
#include "functionality\overwatch.h"

#include "QtWidgets\qshortcut.h"

#include "ui_general_main_page.h"


GeneralMainPage::GeneralMainPage(Settings *settings, std::string const &number, WindowType const &type, QWidget *parent)
  : ParentPage("OfferPage", parent)
  , m_ui(new Ui::generalMainPage)
  , m_hourlyRate(settings->hourlyRate)
  , m_defaultHeadline(QString::fromStdString(settings->defaultHeadline))
  , m_defaultEndline(QString::fromStdString(settings->defaultEndline))
{
  if (type == WindowType::WindowTypeOffer)
  {
    m_internalData = new OfferData();
  }
  else
  {
    m_internalData = new InvoiceData();
  }
  m_ui->setupUi(this);
  m_ui->editHeading->setTabChangesFocus(true);
  m_ui->editEnding->setTabChangesFocus(true);

  connect(m_ui->editNumber, &QLineEdit::textChanged, [this](QString txt)
  {
    m_internalData->number = txt;
  });
  connect(m_ui->editDate, &QLineEdit::textChanged, [this](QString txt)
  {
    m_internalData->date = txt;
  });
  connect(m_ui->editCustomerNumber, &QLineEdit::textChanged, [this](QString txt)
  {
    m_internalData->customerNumber = txt;
  });
  connect(m_ui->editSalutation, &QLineEdit::textChanged, [this](QString txt)
  {
    m_internalData->salutation = txt;
  });
  connect(m_ui->editName, &QLineEdit::textChanged, [this](QString txt)
  {
    m_internalData->name = txt;
  });
  connect(m_ui->editStreet, &QLineEdit::textChanged, [this](QString txt)
  {
    m_internalData->street = txt;
  });
  connect(m_ui->editPlace, &QLineEdit::textChanged, [this](QString txt)
  {
    m_internalData->place = txt;
  });
  connect(m_ui->editSubject, &QLineEdit::textChanged, [this](QString txt)
  {
    m_internalData->subject = txt;
  });
  connect(m_ui->editSkonto, &QLineEdit::textChanged, [this](QString txt)
  {
    m_internalData->skonto = txt.toDouble();
  });
  connect(m_ui->editPayNormal, &QLineEdit::textChanged, [this](QString txt)
  {
    m_internalData->payNormal = txt.toDouble();
  });
  connect(m_ui->editPaySkonto, &QLineEdit::textChanged, [this](QString txt)
  {
    m_internalData->paySkonto = txt.toDouble();
  });
  connect(m_ui->editHourlyRate, &QLineEdit::textChanged, [this](QString txt)
  {
    m_internalData->hourlyRate = txt.toDouble();
  });
  connect(m_ui->editHeading, &QTextEdit::textChanged, [this]()
  {
    m_internalData->headline = m_ui->editHeading->toPlainText();;
  });
  connect(m_ui->editEnding, &QTextEdit::textChanged, [this]()
  {
    m_internalData->endline = m_ui->editEnding->toPlainText();;
  });
  connect(m_ui->buttonHeading, &QPushButton::clicked, [this]()
  {
    m_ui->editHeading->setText(m_defaultHeadline);
  });
  connect(m_ui->buttonEnding, &QPushButton::clicked, [this]()
  {
    m_ui->editEnding->setText(m_defaultEndline);
  });
  m_ui->editNumber->setText(QString::fromStdString(number));
  m_ui->editHourlyRate->setText(QString::number(m_hourlyRate));

  new QShortcut(QKeySequence(Qt::Key_F1), this, SLOT(TakeFromAdress()));
}

GeneralMainPage::~GeneralMainPage()
{
  delete m_internalData;
}

void GeneralMainPage::TakeFromAdress()
{
  Overwatch &tabs = Overwatch::GetInstance();
  auto tab = tabs.GetTabPointer(TabName::AddressTab);
  if (tab == nullptr)
  {
    throw std::runtime_error("Tab not found in overwatch");
  }

  auto artNumbers = tab->GetArtNumbers();
  ShowValueList *dia = new ShowValueList(artNumbers, this);
  if (dia->exec() == QDialog::Accepted)
  {
    QString chosenCustomer = dia->currentItem;
    auto data = static_cast<AddressData*>(tab->GetData(chosenCustomer.toStdString()));
    if (data == nullptr)
    {
      throw std::runtime_error("Adress data not found");
    }
    m_ui->editCustomerNumber->setText(QString::number(data->number));
    m_ui->editSalutation->setText(data->salutation);
    m_ui->editName->setText(data->name);
    m_ui->editStreet->setText(data->street);
    m_ui->editPlace->setText(data->plz + " " + data->city);
  }
}

void GeneralMainPage::SetData(GeneralMainData *data)
{
  m_ui->editNumber->setText(data->number);
  m_ui->editDate->setText(data->date);
  m_ui->editCustomerNumber->setText(data->customerNumber);
  m_ui->editSalutation->setText(data->salutation);
  m_ui->editName->setText(data->name);
  m_ui->editStreet->setText(data->street);
  m_ui->editPlace->setText(data->place);
  m_ui->editSkonto->setText(QString::number(data->skonto));
  m_ui->editPayNormal->setText(QString::number(data->payNormal));
  m_ui->editPaySkonto->setText(QString::number(data->paySkonto));
  m_ui->editHourlyRate->setText(QString::number(data->hourlyRate));
  m_ui->editHeading->setText(data->headline);
  m_ui->editEnding->setText(data->endline);
}
