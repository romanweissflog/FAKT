#include "pages\single_invoice.h"
#include "functionality\overwatch.h"
#include "pages\invoice_page.h"


SingleInvoice::SingleInvoice(size_t number, std::string const &tableName, QWidget *parent)
  : SingleEntry(number, tableName, PrintType::PrintTypeSingleInvoice, parent)
{
  this->setWindowTitle("Rechnung");
}

void SingleInvoice::Calculate()
{
  data.total = data.materialTotal + data.helperTotal + data.serviceTotal;
  data.mwstTotal = data.total / 100 * data.mwst;
  data.brutto = data.total + data.mwstTotal;
  data.skontoTotal = data.brutto / 100 * data.skonto + data.brutto;
}

void SingleInvoice::EditMeta()
{
  std::string number = std::to_string(m_number);
  auto tab = Overwatch::GetInstance().GetTabPointer(TabName::InvoiceTab);
  InvoicePage *editPage = new InvoicePage(m_settings, number, TabName::InvoiceTab);
  InvoiceData *data = static_cast<InvoiceData*>(tab->GetData(number));
  editPage->SetData(data);
  if (editPage->exec() == QDialog::Accepted)
  {
    tab->SetData(editPage->data);
  }
}