#include "tabs\payment.h"
#include "pages\payment_page.h"
#include "functionality\overwatch.h"
#include "functionality\sql_helper.hpp"

#include "ui_basetab.h"

#include "QtWidgets\qshortcut.h"
#include "QtCore\qdebug.h"
#include "QtSql\qsqlerror.h"

namespace
{
  TabData tabData
  {
    "Payment",
    "RECHNUNG",
    "Rechnungen:Zahlungen",
    "RENR",
    PrintType::PrintTypePayment,
    {
      { "RENR", "Re.-Nr." },
      { "REDAT", "Datum" },
      { "KUNR", "K.-Nr." },
      { "NAME", "Kunde" },
      { "GESAMT", "Netto" },
      { "BRUTTO", "Brutto" },
      { "ANREDE", "Anrede" },
      { "STRASSE", QString::fromStdString("Stra" + german::ss + "e") },
      { "ORT", "Ort" },
      { "MGESAMT", "Material" },
      { "LGESAMT", "Leistung" },
      { "SGESAMT", "S-Zeug" },
      { "MWSTGESAMT", "MwstGesamt" },
      { "SKONTO", "Skonto" },
      { "SKBETRAG", "m.Sk.abzug" },
      { "BEZAHLT", "Bezahlt" },
      { "HEADLIN", "Header" },
      { "BEZADAT", "Datum d. Zahlung" },
      { "LIEFDAT", "Lieferdatum" },
      { "Z_FRIST_N", "Zahlung normal" },
      { "Z_FRIST_S", "Zahlung Skonto" },
      { "SCHLUSS", "Schluss" },
      { "STUSATZ", "Stundensatz" },
      { "BETREFF", "Betreff" },
      { "MWSTSATZ", "Mwst" }
    },
    { "RENR", "NAME", "BRUTTO", "SKBETRAG", "BEZAHLT", "BEZADAT" }
  };
}

Payment::Payment(QSqlQuery &query, QWidget *parent)
  : BaseTab(tabData, parent)
{
  m_ui->deleteEntry->setEnabled(false);
  m_ui->editEntry->setEnabled(false);
  m_ui->newEntry->setEnabled(false);
  m_ui->pdfExport->setEnabled(false);
  m_ui->printEntry->setEnabled(false);

  QPushButton *payment = new QPushButton("Zahlungseingang (Z)", this);
  connect(payment, &QPushButton::clicked, this, &Payment::HandlePayment);
  m_ui->layoutAction->addWidget(payment);

  delete m_shortCuts[Qt::Key_N];
  delete m_shortCuts[Qt::Key_M];
  delete m_shortCuts[Qt::Key_L];
  delete m_shortCuts[Qt::Key_P];
  delete m_shortCuts[Qt::Key_D];

  new QShortcut(QKeySequence(Qt::Key_Z), this, SLOT(HandlePayment()));
}

Payment::~Payment()
{}

void Payment::HandlePayment()
{
  auto const index = m_ui->databaseView->currentIndex();
  if (index.row() == -1 || index.column() == -1)
  {
    return;
  }
  QString const schl = m_ui->databaseView->model()->data(index.model()->index(index.row(), 0)).toString();
  QString const tabName = "Rechnungen:Zahlungen:" + schl;
  PaymentPage *page = new PaymentPage(m_query, schl, this);

  AddSubtab(page, tabName);
  connect(page, &PageFramework::Accepted, [this, page, tabName]()
  {
    auto const sql = GenerateInsertCommand("ZAHLUNG",
      SqlPair("RENR", page->content->data->number),
      SqlPair("BEZAHLT", page->content->newPaid),
      SqlPair("BEZADAT", page->content->data->payDate));
    m_rc = m_query.prepare(QString::fromStdString(sql));
    if (!m_rc)
    {
      Log::GetLog().Write(LogType::LogTypeError, m_logId, m_query.lastError().text().toStdString());
      return;
    }
    m_rc = m_query.exec();
    if (!m_rc)
    {
      Log::GetLog().Write(LogType::LogTypeError, m_logId, m_query.lastError().text().toStdString());
      return;
    }

    Overwatch::GetInstance().GetTabPointer(TabName::InvoiceTab)->SetData(page->content->data);
    
    ShowDatabase();
    emit CloseTab(tabName);
  });
  connect(page, &PageFramework::Declined, [this, tabName]()
  {
    emit CloseTab(tabName);
  });
}
