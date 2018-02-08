#include "pages\service_page.h"

#include "QtCore\qdebug.h"
#include "QtSql\qsqlerror.h"

#include "ui_service_page.h"


ServicePage::ServicePage(Settings *settings,
  QSqlQuery &query,
  QString const &edit,
  QWidget *parent)
  : ParentPage("ServicePage", parent)
  , m_ui(new Ui::servicePage)
  , m_euroPerMin(settings->hourlyRate / 60.0)
  , m_query(query)
{
  m_ui->setupUi(this);
  data = {};
  connect(m_ui->editKey, &QLineEdit::textChanged, [this](QString txt)
  {
    data.key = txt;
  });
  connect(m_ui->editDescr, &QTextEdit::textChanged, [this]()
  {
    data.description = m_ui->editDescr->toPlainText();
  });
  connect(m_ui->editUnit, &QLineEdit::textChanged, [this](QString txt)
  {
    data.unit = txt;
  });
  connect(m_ui->editServicePeriod, &QLineEdit::textChanged, [this](QString txt)
  {
    data.minutes = txt.toDouble();
    double price = data.minutes * m_euroPerMin;
    data.service = price;
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

  m_ui->copyBox->addItem("");
  if (!m_query.exec("SELECT ARTNR FROM LEISTUNG"))
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, m_query.lastError().text().toStdString());
    return;
  }
  while (m_query.next())
  {
    m_ui->copyBox->addItem(m_query.value(0).toString());
  }
  if (edit.size() > 0)
  {
    CopyData(edit);
  }
}

ServicePage::~ServicePage()
{}

void ServicePage::Calculate()
{
  double value = data.service + data.material + data.helperMaterial;
  m_ui->labelTotal->setText(QString::number(value));
  data.ep = value;
}

void ServicePage::CopyData(QString txt)
{
  if (m_ui->copyBox->currentIndex() == 0 && txt.size() == 0)
  {
    return;
  }
  if (!m_query.prepare("SELECT * FROM LEISTUNG WHERE ARTNR = :ID"))
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, m_query.lastError().text().toStdString());
    return;
  }
  m_query.bindValue(":ID", txt);
  if (!m_query.exec())
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, m_query.lastError().text().toStdString());
    return;
  }
  m_query.next();
  m_ui->editKey->setText(m_query.value(1).toString());
  m_ui->editDescr->setText(m_query.value(2).toString());
  m_ui->editMatPrice->setText(m_query.value(3).toString());
  m_ui->editServicePeriod->setText(m_query.value(4).toString());
  m_ui->labelServicePrice->setText(m_query.value(5).toString());
  m_ui->editHelperMatPrice->setText(m_query.value(6).toString());
  m_ui->labelTotal->setText(m_query.value(7).toString());
  m_ui->editUnit->setText(m_query.value(8).toString());
  m_ui->editMatEkp->setText(m_query.value(9).toString());
}

void ServicePage::SetFocusToFirst()
{
  m_ui->editKey->setFocus();
}
