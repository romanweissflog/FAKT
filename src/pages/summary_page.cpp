#include "pages\summary_page.h"

#include "ui_summary_page.h"

#include "QtWidgets\qshortcut.h"
#include "QtCore\qdebug.h"
#include "QtSql\qsqlerror.h"

SummaryPage::SummaryPage(GeneralMainData const &data, 
  QSqlQuery &query, 
  QString const &table, 
  QWidget *parent)
  : QWidget(parent)
  , m_ui(new Ui::summaryPage)
  , m_query(query)
  , m_table(table)
{
  m_ui->setupUi(this);
  connect(m_ui->buttonGroups, &QPushButton::clicked, this, &SummaryPage::CalculateGroups);

  connect(new QShortcut(QKeySequence(Qt::Key_Escape), this), &QShortcut::activated, [this]()
  {
    emit Close();
  });

  SetMainData(data);
  CalculateDetailData(data.hourlyRate);
}

SummaryPage::~SummaryPage()
{
}

void SummaryPage::SetMainData(GeneralMainData const &data)
{
  m_ui->labelMaterial->setText(QString::number(data.materialTotal));
  m_ui->labelService->setText(QString::number(data.serviceTotal));
  m_ui->labelHelper->setText(QString::number(data.helperTotal));
  m_ui->labelNetto->setText(QString::number(data.total));
  m_ui->labelMwst->setText(QString::number(data.mwstTotal));
  m_ui->labelBrutto->setText(QString::number(data.brutto));

  m_ui->labelHourlyRate->setText(QString::number(data.hourlyRate));
}

void SummaryPage::CalculateDetailData(double hourlyRate)
{
  QString sql = "SELECT MENGE, EKP, MP, BAUZEIT FROM " + m_table;
  auto rc = m_query.exec(sql);
  if (!rc)
  {
    qDebug() << m_query.lastError();
  }

  double ekp{};
  double ep{};
  double time{};
  while (m_query.next())
  {
    double number = m_query.value(0).toDouble();
    ekp += number * m_query.value(1).toDouble();
    ep += number * m_query.value(2).toDouble();
    time += number * m_query.value(3).toDouble();
  }
  double profit = ep - ekp;
  m_ui->labelMaterialEKP->setText(QString::number(ekp));
  m_ui->labelMaterialEP->setText(QString::number(ep));
  m_ui->labelProfitTotal->setText(QString::number(profit));
  m_ui->labelProfitPerc->setText(QString::number(100.0 * profit / ep));
  m_ui->labelServiceMinutes->setText(QString::number(time));
  m_ui->labelServiceHours->setText(QString::number(time / 60.0));
  m_ui->labelServiceDays->setText(QString::number(time / 480.0));
  m_ui->labelServiceTotal->setText(QString::number(time / 60.0 * hourlyRate));
}

void SummaryPage::CalculateGroups()
{
}