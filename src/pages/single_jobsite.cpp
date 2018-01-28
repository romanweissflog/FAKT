#include "pages\single_jobsite.h"
#include "pages\invoice_page.h"
#include "functionality\overwatch.h"


SingleJobsite::SingleJobsite(size_t number, std::string const &tableName, QWidget *parent)
  : SingleEntry(number, tableName, TabName::JobsiteTab, parent)
  , data(static_cast<InvoiceData*>(m_internalData.get()))
{
  m_data.tabName = "Baustellen";
  this->setWindowTitle("Baustelle");
}

void SingleJobsite::Calculate()
{
  data->total = data->materialTotal + data->helperTotal + data->serviceTotal;
  data->mwstTotal = data->total / 100 * data->mwst;
  data->brutto = data->total + data->mwstTotal;
  data->skonto = data->brutto / 100 * data->skonto + data->brutto;
}

void SingleJobsite::EditMeta()
{
  QString number = QString::number(m_number);
  auto tab = Overwatch::GetInstance().GetTabPointer(TabName::JobsiteTab);
  InvoicePage *editPage = new InvoicePage(m_settings, number, TabName::JobsiteTab);
  std::unique_ptr<InvoiceData> data(static_cast<InvoiceData*>(tab->GetData(number.toStdString()).release()));
  editPage->SetData(data.get());  
  
  QString const tabName = m_data.tabName + ":" + QString::number(m_number) + ":Allgemein";
  editPage->hide();
  AddSubtab(editPage, tabName);
  if (editPage->exec() == QDialog::Accepted)
  {
    std::unique_ptr<Data> data(editPage->data);
    tab->SetData(data);
  }
  CloseTab(tabName);
}