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
  connect(m_ui->editMainDescription, &QLineEdit::textChanged, [this](QString txt)
  {
    data.mainDescription = txt;
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
    QLocale l(QLocale::German);
    data.minutes = l.toDouble(txt);
    double price = data.minutes * m_euroPerMin;
    data.service = price;
    m_ui->labelServicePrice->setText(l.toString(price, 'f', 2));
    Calculate();
  });
  connect(m_ui->editMatPrice, &QLineEdit::textChanged, [this](QString txt)
  {
    QLocale l(QLocale::German);
    data.material = l.toDouble(txt);
    m_ui->editMatEkp->setText(l.toString(data.material, 'f', 2));
    Calculate();
  });
  connect(m_ui->editHelperMatPrice, &QLineEdit::textChanged, [this](QString txt)
  {
    QLocale l(QLocale::German);
    data.helperMaterial = l.toDouble(txt);
    Calculate();
  });
  connect(m_ui->editMatEkp, &QLineEdit::textChanged, [this](QString txt)
  {
    QLocale l(QLocale::German);
    data.ekp = l.toDouble(txt);
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
  QLocale l(QLocale::German);
  double value = data.service + data.material + data.helperMaterial;
  m_ui->labelTotal->setText(l.toString(value, 'f', 2));
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

  QLocale l(QLocale::German);
  m_ui->editKey->setText(m_query.value(1).toString());
  m_ui->editMainDescription->setText(m_query.value(2).toString());
  m_ui->editDescr->setText(m_query.value(3).toString());
  m_ui->editMatPrice->setText(l.toString(m_query.value(4).toDouble(), 'f', 2));
  m_ui->editServicePeriod->setText(l.toString(m_query.value(5).toDouble(), 'f', 2));
  m_ui->labelServicePrice->setText(l.toString(m_query.value(6).toDouble(), 'f', 2));
  m_ui->editHelperMatPrice->setText(l.toString(m_query.value(7).toDouble(), 'f', 2));
  m_ui->labelTotal->setText(l.toString(m_query.value(8).toDouble(), 'f', 2));
  m_ui->editUnit->setText(m_query.value(9).toString());
  m_ui->editMatEkp->setText(l.toString(m_query.value(10).toDouble(), 'f', 2));
}

void ServicePage::SetFocusToFirst()
{
  m_ui->editKey->setFocus();
}
