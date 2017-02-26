#include "adding_pages.h"

#include "ui_leistung_page.h"

LeistungPage::LeistungPage(Settings &settings, QWidget *parent)
  : QDialog(parent)
  , m_ui(new Ui::leistungPage)
  , m_euroPerMin(settings.euroPerMin)
{
  m_ui->setupUi(this);
  data = {};
  connect(m_ui->editKey, &QLineEdit::textChanged, [this](QString txt)
  {
    data.key = txt;
  });
  connect(m_ui->editDescr, &QLineEdit::textChanged, [this](QString txt)
  {
    data.description = txt;
  });
  connect(m_ui->editUnit, &QLineEdit::textChanged, [this](QString txt)
  {
    data.unit = txt;
  });
  connect(m_ui->editServicePeriod, &QLineEdit::textChanged, [this](QString txt)
  {
    data.minutes = txt.toDouble();
    double price = data.minutes*m_euroPerMin;
    m_ui->labelServicePrice->setText(QString::number(price));
    Calculate();
  });
  connect(m_ui->editMatPrice, &QLineEdit::textChanged, [this](QString txt)
  {
    data.material = txt.toDouble();
    Calculate();
  });
  connect(m_ui->editHelperMatPrice, &QLineEdit::textChanged, [this](QString txt)
  {
    data.helperMaterial = txt.toDouble();
    Calculate();
  });
  connect(m_ui->editMatEkp, &QLineEdit::textChanged, [this](QString txt)
  {
    data.ekp = txt.toDouble();
  });
}

LeistungPage::~LeistungPage()
{}

void LeistungPage::Calculate()
{
  double value = data.service + data.material + data.helperMaterial;
  m_ui->labelTotal->setText(QString::number(value));
  data.ep = value;
}

void LeistungPage::CopyData()
{}