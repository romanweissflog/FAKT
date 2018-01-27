#include "pages\single_invoice.h"
#include "functionality\overwatch.h"
#include "pages\invoice_page.h"


SingleInvoice::SingleInvoice(size_t number, std::string const &tableName, QWidget *parent)
  : SingleEntry(number, tableName, TabName::InvoiceTab, parent)
  , data(static_cast<InvoiceData*>(m_internalData.get()))
{
  this->setWindowTitle("Rechnung");
}

void SingleInvoice::Calculate()
{
  data->total = data->materialTotal + data->helperTotal + data->serviceTotal;
  data->mwstTotal = data->total / 100 * data->mwst;
  data->brutto = data->total + data->mwstTotal;
  data->skontoTotal = data->brutto / 100 * data->skonto + data->brutto;
}

void SingleInvoice::EditMeta()
{
  std::string number = std::to_string(m_number);
  auto tab = Overwatch::GetInstance().GetTabPointer(TabName::InvoiceTab);
  InvoicePage *editPage = new InvoicePage(m_settings, number, TabName::InvoiceTab);

  std::unique_ptr<InvoiceData> data(static_cast<InvoiceData*>(tab->GetData(number).release()));
  editPage->SetData(data.get());
  if (editPage->exec() == QDialog::Accepted)
  {
    std::unique_ptr<Data> data(editPage->data);
    tab->SetData(data);
  }
}