#include "pages\single_invoice.h"
#include "functionality\overwatch.h"
#include "pages\invoice_page.h"


SingleInvoice::SingleInvoice(size_t number, QWidget *parent)
  : SingleEntry(number, "R", TabName::InvoiceTab, parent)
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
  data->skontoTotal = data->brutto - data->brutto / 100 * data->skonto;
}

void SingleInvoice::Recalculate(Data *edited)
{
  InvoiceData *editedData = reinterpret_cast<InvoiceData*>(edited);
  data->mwstTotal = data->total / 100 * editedData->mwst;
  data->brutto = data->total + data->mwstTotal;
  data->skontoTotal = data->brutto - data->brutto / 100 * editedData->skonto;
  SingleEntry::Recalculate(edited);
}

void SingleInvoice::EditMeta()
{
  QString number = QString::number(m_number);
  auto tab = Overwatch::GetInstance().GetTabPointer(TabName::InvoiceTab);
  InvoicePage *editPage = new InvoicePage(m_settings, number, TabName::InvoiceTab);
  connect(editPage, &PageFramework::AddExtraPage, [this, editPage, number](QWidget *widget, QString const &txt)
  {
    emit AddSubtab(widget, "Angebote:" + number + ":Allgemein:" + txt);
  });
  connect(editPage, &PageFramework::CloseExtraPage, [this, editPage, number](QString const &txt)
  {
    emit CloseTab("Angebote:" + number + ":Allgemein:" + txt);
  });

  std::unique_ptr<InvoiceData> data(static_cast<InvoiceData*>(tab->GetData(number.toStdString()).release()));
  if (!data)
  {
    return;
  }
  editPage->content->SetData(data.get());
  editPage->content->LockNumber();

  QString const tabName = m_data.tabName + ":" + QString::number(m_number) + ":Allgemein";
  AddSubtab(editPage, tabName);
  connect(editPage, &PageFramework::Accepted, [this, tab, editPage, tabName]()
  {
    Recalculate(editPage->content->data);
    tab->SetData(editPage->content->data);
    emit CloseTab(tabName);
  });
  connect(editPage, &PageFramework::Declined, [this, tabName]()
  {
    emit CloseTab(tabName);
  });
}

void SingleInvoice::SetLastData(Data *input)
{
  SingleEntry::SetLastData(input);
  InvoiceData *invoiceData = static_cast<InvoiceData*>(input);
  data->skontoTotal = invoiceData->skontoTotal;
  data->paid = invoiceData->paid;
  data->payDate = invoiceData->payDate;
  data->deliveryDate = invoiceData->deliveryDate;
  data->mwst = invoiceData->mwst;
}
