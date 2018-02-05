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
  data->skontoTotal = data->brutto / 100 * data->skonto + data->brutto;
}

void SingleJobsite::Recalculate(Data *edited)
{
  InvoiceData *editedData = reinterpret_cast<InvoiceData*>(edited);
  data->mwstTotal = data->total / 100 * editedData->mwst;
  data->brutto = data->total + data->mwstTotal;
  data->skontoTotal = data->brutto / 100 * editedData->skonto + data->brutto;
  SingleEntry::Recalculate(edited);
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
  editPage->SetFocusToFirst();
  editPage->LockNumber();
  if (editPage->exec() == QDialog::Accepted)
  {
    std::unique_ptr<Data> data(editPage->data);
    Recalculate(editPage->data);
    tab->SetData(editPage->data);
  }
  CloseTab(tabName);
}

void SingleJobsite::SetLastData(Data *input)
{
  SingleEntry::SetLastData(input);
  InvoiceData *invoiceData = static_cast<InvoiceData*>(input);
  data->skontoTotal = invoiceData->skontoTotal;
  data->paid = invoiceData->paid;
  data->payDate = invoiceData->payDate;
  data->deliveryDate = invoiceData->deliveryDate;
  data->mwst = invoiceData->mwst;
}
