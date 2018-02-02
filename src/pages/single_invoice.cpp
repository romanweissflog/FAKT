#include "pages\single_invoice.h"
#include "functionality\overwatch.h"
#include "pages\invoice_page.h"


SingleInvoice::SingleInvoice(size_t number, std::string const &tableName, QWidget *parent)
  : SingleEntry(number, tableName, TabName::InvoiceTab, parent)
  , data(static_cast<InvoiceData*>(m_internalData.get()))
{
  m_data.tabName = "Rechnungen";
  this->setWindowTitle("Rechnung");
}

void SingleInvoice::Calculate()
{
  data->total = data->materialTotal + data->helperTotal + data->serviceTotal;
  data->mwstTotal = data->total / 100 * data->mwst;
  data->brutto = data->total + data->mwstTotal;
  data->skontoTotal = data->brutto / 100 * data->skonto + data->brutto;
}

void SingleInvoice::Recalculate(std::unique_ptr<Data> &edited)
{
  InvoiceData *editedData = reinterpret_cast<InvoiceData*>(edited.get());
  data->mwstTotal = data->total / 100 * editedData->mwst;
  data->brutto = data->total + data->mwstTotal;
  data->skonto = data->brutto / 100 * editedData->skonto + data->brutto;
  SingleEntry::Recalculate(edited);
}

void SingleInvoice::EditMeta()
{
  QString number = QString::number(m_number);
  auto tab = Overwatch::GetInstance().GetTabPointer(TabName::InvoiceTab);
  InvoicePage *editPage = new InvoicePage(m_settings, number, TabName::InvoiceTab);

  std::unique_ptr<InvoiceData> data(static_cast<InvoiceData*>(tab->GetData(number.toStdString()).release()));
  editPage->SetData(data.get());

  QString const tabName = m_data.tabName + ":" + QString::number(m_number) + ":Allgemein";
  editPage->hide();
  AddSubtab(editPage, tabName);
  editPage->setFocus();
  if (editPage->exec() == QDialog::Accepted)
  {
    std::unique_ptr<Data> data(editPage->data);
    Recalculate(data);
    tab->SetData(data);
  }
  CloseTab(tabName);
  emit CloseTab(tabName);
}