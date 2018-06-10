#include "tabs\general.h"
#include "pages\invoice_page.h"
#include "pages\single_invoice.h"
#include "pages\offer_page.h"
#include "pages\single_offer.h"
#include "pages\single_jobsite.h"

#include "ui_basetab.h"

#include "QtSql\qsqlerror.h"
#include "QtSql\qsqlquerymodel.h"

namespace
{
  PageFramework* GetPage(TabName const &tab, Settings *settings, QString const &number, GeneralTab *parent)
  {
    switch (tab)
    {
    case TabName::InvoiceTab: 
    case TabName::JobsiteTab:
      return new InvoicePage(settings, number, tab, parent); break;
    case TabName::OfferTab:
      return new OfferPage(settings, number, parent); break;
    default: throw std::runtime_error("Bad tab for General GetPage");
    }
  }

  SingleEntry* GetSinglePage(TabName const &tab, QString const &key, GeneralTab *parent)
  {
    switch (tab)
    {
    case TabName::InvoiceTab: return new SingleInvoice(key.toULongLong(), parent); break;
    case TabName::JobsiteTab: return new SingleJobsite(key.toULongLong(), parent); break;
    case TabName::OfferTab: return new SingleOffer(key.toULongLong(), parent); break;
    default: throw std::runtime_error("Invalid single page");
    }
  }
}

GeneralTab::GeneralTab(TabData const &tabData, QWidget *parent)
  : BaseTab(tabData, parent)
{
}

void GeneralTab::AddEntry()
{
  QString number = QString::number(std::stoul(m_settings->lastInvoice) + 1);
  InvoicePage *page = new InvoicePage(m_settings, number, TabName::InvoiceTab, this);
  connect(page, &PageFramework::AddExtraPage, [this, page](QWidget *widget, QString const &txt)
  {
    emit AddSubtab(widget, m_data.tabName + ":Neu:" + txt);
  });
  connect(page, &PageFramework::CloseExtraPage, [this, page](QString const &txt)
  {
    emit CloseTab(m_data.tabName + ":Neu:" + txt);
  });
  emit AddSubtab(page, m_data.tabName + ":Neu");
  connect(page, &PageFramework::Accepted, [this, page]()
  {
    auto &&data = page->GetData();
    AddData(data);
    ShowDatabase();
    emit CloseTab(m_data.tabName + ":Neu");
  });
  connect(page, &PageFramework::Declined, [&]()
  {
    emit CloseTab(m_data.tabName + ":Neu");
  });
}

void GeneralTab::EditEntry()
{
  auto index = m_ui->databaseView->currentIndex();
  if (index.row() == -1 || index.column() == -1)
  {
    return;
  }
  QString const key = m_ui->databaseView->model()->data(index.model()->index(index.row(), 0)).toString();

  SingleEntry *page = GetSinglePage(m_data.tabType, key, this);
  page->SetSettings(m_settings);
  page->SetDatabase(m_data.dataDatabase);
  auto data = GetData(key.toStdString());
  page->SetLastData(data);

  page->hide();
  emit AddSubtab(page, m_data.tabName + key);

  connect(page, &SingleInvoice::UpdateData, [this, page, key]()
  {
    auto data = page->GetInternalData();
    EditData(key, data);
  });
}

void GeneralTab::DeleteDataTable(QString const &key)
{
  try
  {
    QSqlDatabase dataDb = QSqlDatabase::addDatabase("QSQLITE", "general");
    dataDb.setDatabaseName(m_data.dataDatabase);

    dataDb.open();
    QSqlQuery dataQuery(dataDb);
    QString const id = util::GetPaddedNumber(key);
    m_rc = dataQuery.prepare("DROP TABLE IF EXISTS " + m_data.dataTableSuffix + id);
    if (!m_rc)
    {
      throw std::runtime_error(dataQuery.lastError().text().toStdString());
    }
    m_rc = dataQuery.exec();
    if (!m_rc)
    {
      throw std::runtime_error(dataQuery.lastError().text().toStdString());
    }
    dataDb.close();
    dataDb = QSqlDatabase();
    dataDb.removeDatabase("general");
  }
  catch (std::runtime_error e)
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, e.what());
  }
}
