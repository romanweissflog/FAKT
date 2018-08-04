#include "pages\summary_page.h"
#include "functionality\overwatch.h"

#include "ui_summary_content.h"
#include "ui_page_framework.h"

#include "QtWidgets\qshortcut.h"
#include "QtCore\qdebug.h"
#include "QtSql\qsqlerror.h"

#include <regex>

SummaryContent::SummaryContent(GeneralMainData const &data,
  QSqlQuery &query,
  QString const &table, 
  double mwst,
  QWidget *parent)
  : ParentPage("SummaryPage", parent)
  , m_ui(new Ui::summaryContent)
  , partialSums(nullptr)
  , correctedData({})
  , m_query(query)
  , m_table(table)
  , m_logId(Log::GetLog().RegisterInstance("SummaryPage"))
  , m_mwst(mwst)
{
  m_ui->setupUi(this);

  SetMainData(data);
  CalculateDetailData(data.hourlyRate);

  SHORTCUT(f5Key, Key_F5, PartialSums)
  SHORTCUT(f6Key, Key_F6, CorrectData)
}

void SummaryContent::SetMainData(GeneralMainData const &data)
{
  correctedData = data;

  m_ui->labelMaterial->setText(QString::number(data.materialTotal));
  m_ui->labelService->setText(QString::number(data.serviceTotal));
  m_ui->labelHelper->setText(QString::number(data.helperTotal));
  m_ui->labelNetto->setText(QString::number(data.total));
  m_ui->labelMwst->setText(QString::number(data.mwstTotal));
  m_ui->labelBrutto->setText(QString::number(data.brutto));
  m_ui->labelHourlyRate->setText(QString::number(data.hourlyRate));
}

void SummaryContent::CalculateDetailData(double hourlyRate)
{
  QString sql = "SELECT MENGE, EKP, MP, BAUZEIT, POSIT FROM " + m_table;
  auto rc = m_query.exec(sql);
  if (!rc)
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, m_query.lastError().text().toStdString());
    return;
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
    std::string pos = m_query.value(4).toString().toStdString();
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
  
  connect(m_ui->buttonGroups, &QPushButton::clicked, this, &SummaryContent::PartialSums);
  connect(m_ui->buttonCorrect, &QPushButton::clicked, this, &SummaryContent::CorrectData);
  m_ui->buttonGroups->installEventFilter(Overwatch::GetInstance().GetEventLogger());
  m_ui->buttonCorrect->installEventFilter(Overwatch::GetInstance().GetEventLogger());
}

void SummaryContent::PartialSums()
{
  QString sql = "SELECT POSIT, HAUPTARTBEZ, GP FROM " + m_table;
  auto rc = m_query.exec(sql);
  if (!rc)
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, m_query.lastError().text().toStdString());
    return;
  }

  try
  {
    PartialSumData data = util::GetPartialSums(m_query);

    std::vector<QString> groups, descriptions, prices;
    for (auto &&d : data)
    {
      groups.push_back(QString::number(d.first));
      descriptions.push_back(d.second.first);
      prices.push_back(QString::number(d.second.second));
    }

    partialSums = new CustomTable("Titelsummen", data.size(), { "Gruppe", "Bezeichnung", "Gesamtpreis" }, this);
    partialSums->SetColumn(0, groups);
    partialSums->SetColumn(1, descriptions);
    partialSums->SetColumn(2, prices);
    partialSums->SetSortingEnabled();
    
    emit AddPage();
    connect(partialSums, &CustomTable::Close, [this]()
    {
      emit ClosePage();
    });
  }
  catch (...)
  {
    QMessageBox::warning(this, tr("Hinweis"),
      tr("Schlecht formatierte Eingangsdaten"));
  }
}

void SummaryContent::CorrectData()
{
  try
  {
    correctedData = GeneralMainData{};
    QString sql = "SELECT MENGE, MP, LP, SP, STUSATZ FROM " + m_table;
    auto rc = m_query.exec(sql);
    if (!rc)
    {
      throw std::runtime_error(m_query.lastError().text().toStdString());
    }
    
    std::regex reg("\\d+");
    std::smatch match;
    std::string str = m_table.toStdString();
    if (!std::regex_search(str, match, reg))
    {
      throw std::runtime_error("regex missmatch for extracting number");
    }
    correctedData.number = QString::number(std::stoul(match[0].str()));

    while (m_query.next())
    {
      double const number = m_query.value(0).toDouble();
      correctedData.materialTotal += number * m_query.value(1).toDouble();
      correctedData.serviceTotal += number * m_query.value(2).toDouble();
      correctedData.helperTotal += number * m_query.value(3).toDouble();
      correctedData.hourlyRate = m_query.value(4).toDouble();
    }
    correctedData.total = correctedData.materialTotal + correctedData.serviceTotal + correctedData.helperTotal;
    correctedData.mwstTotal = (100.0 + m_mwst) / 100.0 * correctedData.total - correctedData.total;
    correctedData.brutto = correctedData.total + correctedData.mwstTotal;

    auto adapt = [](QLabel *lbl, double newValue)
    {
      auto const oldText = lbl->text();
      lbl->setText(oldText + " -> " + QString::number(newValue));
    };
    adapt(m_ui->labelMaterial, correctedData.materialTotal);
    adapt(m_ui->labelService, correctedData.serviceTotal);
    adapt(m_ui->labelHelper, correctedData.helperTotal);
    adapt(m_ui->labelNetto, correctedData.total);
    adapt(m_ui->labelMwst, correctedData.mwstTotal);
    adapt(m_ui->labelBrutto, correctedData.brutto);
  }
  catch (std::runtime_error e)
  {
    Log::GetLog().Write(LogTypeError, m_logId, e.what());
  }
}

void SummaryContent::SetFocusToFirst()
{
  this->setFocus();
}


SummaryPage::SummaryPage(GeneralMainData const &data,
  QSqlQuery &query,
  QString const &table,
  double mwst, 
  QWidget *parent)
  : PageFramework(parent)
  , content(new SummaryContent(data, query, table, mwst, this))
{
  m_ui->mainLayout->replaceWidget(m_ui->content, content);

  content->setFocus();
  content->SetFocusToFirst();

  connect(content, &SummaryContent::AddPage, [this]()
  {
    emit AddExtraPage(content->partialSums, "Teilsummen");
  });
  connect(content, &SummaryContent::ClosePage, [this]()
  {
    emit CloseExtraPage("Teilsummen");
    content->setFocus();
  });
}
