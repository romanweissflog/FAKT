#include "pages/percentage_page.h"

#include "ui_percentage_content.h"
#include "ui_page_framework.h"


PercentageContent::PercentageContent(Settings *settings, 
  QString const &type, 
  GeneralMainData const &data, 
  QWidget *parent)
  : m_ui(new Ui::percentageContent)
  , data(data)
  , m_mwst(settings->mwst)
  , percentageMaterial(0.0)
  , percentageService(0.0)
  , m_inputMaterial(data.materialTotal)
  , m_inputService(data.serviceTotal)
{
  m_ui->setupUi(this);
  m_ui->labelType->setText(type + " - Nummer:");
  m_ui->labelNumber->setText(data.number);
  m_ui->labelCustomer->setText(data.name);
  m_ui->labelStreet->setText(data.street);
  m_ui->labelPlace->setText(data.place);
  m_ui->editMaterial->setText("0,0");
  m_ui->editService->setText("0,0");

  connect(m_ui->editService, &QLineEdit::textChanged, [this](QString txt)
  {
    QLocale l(QLocale::German);
    percentageService = l.toDouble(txt);
    Calculate();
  });
  connect(m_ui->editMaterial, &QLineEdit::textChanged, [this](QString txt)
  {
    QLocale l(QLocale::German);
    percentageMaterial = l.toDouble(txt);
    Calculate();
  });
  connect(m_ui->buttonUndo, &QPushButton::clicked, [this]()
  {
    m_ui->editMaterial->setText("0,0");
    m_ui->editService->setText("0,0");
  });
  Calculate();
}

PercentageContent::~PercentageContent()
{
}

void PercentageContent::SetFocusToFirst()
{
  m_ui->editService->setFocus();
}

void PercentageContent::Calculate()
{
  QLocale l(QLocale::German);
  data.serviceTotal = m_inputService * (100.0 + percentageService) / 100.0;
  data.materialTotal = m_inputMaterial * (100.0 + percentageMaterial) / 100.0;
  m_ui->labelService->setText(l.toString(data.serviceTotal, 'f', 2));
  m_ui->labelMaterial->setText(l.toString(data.materialTotal, 'f', 2));

  double const hours = data.serviceTotal / data.hourlyRate;
  m_ui->labelMinutes->setText(l.toString(60.0 * hours, 'f', 2));
  m_ui->labelHours->setText(l.toString(hours, 'f', 2));
  m_ui->labelDays->setText(l.toString(hours / 8.0, 'f', 2));

  data.total = data.serviceTotal + data.materialTotal + data.helperTotal;
  data.mwstTotal = data.total * (100.0 + m_mwst) / 100.0 - data.total;
  data.brutto = data.total + data.mwstTotal;
  m_ui->labelNetto->setText(l.toString(data.total, 'f', 2));
  m_ui->labelBrutto->setText(l.toString(data.brutto, 'f', 2));
}


PercentagePage::PercentagePage(Settings *settings, 
  QString const &type, 
  GeneralMainData const &data, 
  QWidget *parent)
  : PageFramework(parent)
  , content(new PercentageContent(settings, type, data, this))
{
  m_ui->mainLayout->replaceWidget(m_ui->content, content);

  content->setFocus();
  content->SetFocusToFirst();
}

PercentagePage::~PercentagePage()
{}
