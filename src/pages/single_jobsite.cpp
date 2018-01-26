#include "pages\single_jobsite.h"
#include "pages\invoice_page.h"
#include "functionality\overwatch.h"


SingleJobsite::SingleJobsite(size_t number, std::string const &tableName, QWidget *parent)
  : SingleEntry(number, tableName, TabName::JobsiteTab, parent)
  , data(static_cast<InvoiceData*>(m_internalData))
{
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
  std::string number = std::to_string(m_number);
  auto tab = Overwatch::GetInstance().GetTabPointer(TabName::JobsiteTab);
  InvoicePage *editPage = new InvoicePage(m_settings, number, TabName::JobsiteTab);
  InvoiceData *data = static_cast<InvoiceData*>(tab->GetData(number));
  editPage->SetData(data);
  if (editPage->exec() == QDialog::Accepted)
  {
    tab->SetData(editPage->data);
  }
}