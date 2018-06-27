#include "pages\payment_page.h"
#include "functionality\overwatch.h"

#include "ui_payment_content.h"
#include "ui_page_framework.h"

#include "QtSql\qsqldatabase.h"
#include "QtSql\qsqlquerymodel.h"
#include "QtCore\qdebug.h"
#include "QtSql\qsqlerror.h"

#include <vector>

namespace
{
  std::vector<std::pair<std::string, QString>> columns
  {
    { "RENR", "Rechn.-Nummer"},
    { "BEZAHLT", "Betrag" },
    { "BEZADAT", "Datum" }
  };
}

PaymentContent::PaymentContent(QSqlQuery &query, QString const &key, QWidget *parent)
  : ParentPage("PaymentPage", parent)
  , m_ui(new Ui::paymentContent)
  , m_query(query)
  , m_paidBefore(0.0)
  , newPaid(0.0)
{
  m_ui->setupUi(this);

  connect(m_ui->editPaid, &QLineEdit::textChanged, [this](QString txt)
  {
    newPaid = txt.toDouble();
    data->paid = newPaid + m_paidBefore;
    CalculateRest();
  });
  connect(m_ui->editDate, &QLineEdit::textChanged, [this](QString txt)
  {
    data->payDate = txt;
    if (util::IsDateValid(txt))
    {
      m_ui->labelErrorDate->setText("");
    }
    else
    {
      m_ui->labelErrorDate->setText(QString::fromStdString("Ung" + german::ue + "ltiges Datum"));
    }
  });
  connect(m_ui->editSkonto, &QLineEdit::textChanged, [this](QString txt)
  {
    data->skonto = txt.toDouble();
    double val = (100.0 - data->skonto) / 100.0 * data->brutto;
    data->skontoTotal = val;
    m_ui->editSkontoTotal->setText(QString::number(val));
    CalculateRest();
  });
  connect(m_ui->editSkontoTotal, &QLineEdit::textChanged, [this](QString txt)
  {
    data->skonto = txt.toDouble();
    if (std::abs(data->brutto) > std::numeric_limits<double>::epsilon())
    {
      double val = (data->brutto - data->skontoTotal) / data->brutto * 100;
      data->skonto = val;
      m_ui->editSkonto->setText(QString::number(val));
      CalculateRest();
    }
  });

  SetData(key);
  LoadOldPayments();
}

PaymentContent::~PaymentContent()
{

}

void PaymentContent::SetData(QString const &key)
{
  auto input = Overwatch::GetInstance().GetTabPointer(TabName::InvoiceTab)->GetData(key.toStdString());
  if (!input)
  {
    return;
  }
  data = static_cast<InvoiceData*>(input.release());
  m_ui->labelNumber->setText(data->number);
  m_ui->labelCustomer->setText(data->name);
  m_ui->labelBrutto->setText(QString::number(data->brutto, 'f', 2));
  m_ui->editSkonto->setText(QString::number(data->skonto));
  m_paidBefore = data->paid;
}

void PaymentContent::CalculateRest()
{
  double val = data->skontoTotal - data->paid;
  m_ui->labelRest->setText(QString::number(val));
}

void PaymentContent::LoadOldPayments()
{
  QSqlQueryModel *model = new QSqlQueryModel(this);
  m_ui->tableView->setModel(model);
  m_ui->tableView->verticalHeader()->setVisible(false);

  std::string sql = "SELECT ";
  for (auto &&s : columns)
  {
    sql += s.first + ", ";
  }
  sql = sql.substr(0, sql.size() - 2);
  sql += " FROM ZAHLUNG WHERE RENR = :ID";
  auto rc = m_query.prepare(QString::fromStdString(sql));
  if (!rc)
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, m_query.lastError().text().toStdString());
    return;
  }
  m_query.bindValue(":ID", data->number);
  rc = m_query.exec();
  if (!rc)
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, m_query.lastError().text().toStdString());
    return;
  }

  model->setQuery(m_query);
  size_t idx = 0;
  for (auto &&s : columns)
  {
    m_ui->tableView->horizontalHeader()->setSectionResizeMode((int)idx, QHeaderView::Stretch);
    model->setHeaderData((int)idx, Qt::Horizontal, s.second);
    ++idx;
  }
  CalculateRest();
}

void PaymentContent::SetFocusToFirst()
{
  m_ui->editPaid->setFocus();
}


PaymentPage::PaymentPage(QSqlQuery &query, QString const &key, QWidget *parent)
  : PageFramework(parent)
  , content(new PaymentContent(query, key, this))
{
  m_ui->mainLayout->replaceWidget(m_ui->content, content);

  content->setFocus();
  content->SetFocusToFirst();
}

PaymentPage::~PaymentPage()
{}

