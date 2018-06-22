#include "pages\single_jobsite.h"
#include "pages\invoice_page.h"
#include "functionality\overwatch.h"


SingleJobsite::SingleJobsite(size_t number, QWidget *parent)
  : SingleEntry(number, "BA", TabName::JobsiteTab, parent)
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
  data->skontoTotal = data->brutto - data->brutto / 100 * data->skonto;
}

void SingleJobsite::Recalculate(Data *edited)
{
  InvoiceData *editedData = reinterpret_cast<InvoiceData*>(edited);
  if (util::IsDevisionByZero(data->hourlyRate))
  {
    throw std::runtime_error("Devision by zero detected");
  }
  double time = 60.0 * data->serviceTotal / data->hourlyRate;
  data->serviceTotal = time / 60.0 * editedData->hourlyRate;
  data->total = data->serviceTotal + data->helperTotal + data->materialTotal;
  data->mwstTotal = data->total / 100 * editedData->mwst;
  data->brutto = data->total + data->mwstTotal;
  data->skontoTotal = data->brutto - data->brutto / 100 * editedData->skonto;
  SingleEntry::Recalculate(edited);
  editedData->skontoTotal = data->skontoTotal;
}

void SingleJobsite::EditAfterImport(ImportWidget *importWidget)
{
  SingleEntry::EditAfterImport(importWidget);
  data->mwstTotal = data->total / 100 * data->mwst;
  data->brutto = data->total + data->mwstTotal;
  data->skontoTotal = data->brutto - data->brutto / 100 * data->skonto;
}

void SingleJobsite::EditMeta()
{
  QString number = QString::number(m_number);
  auto tab = Overwatch::GetInstance().GetTabPointer(TabName::JobsiteTab);
  InvoicePage *editPage = new InvoicePage(m_settings, number, TabName::JobsiteTab);
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
  editPage->hide();
  AddSubtab(editPage, tabName);
  connect(editPage, &PageFramework::Accepted, [this, tab, editPage, tabName]()
  {
    try
    {
      Recalculate(editPage->content->data);
      tab->SetData(editPage->content->data);
      AdaptPositions(QString::number(m_number));
    }
    catch (std::runtime_error e)
    {
      Log::GetLog().Write(LogType::LogTypeError, m_logId, e.what());
    }
    emit CloseTab(tabName);
  });
  connect(editPage, &PageFramework::Declined, [this, tabName]()
  {
    emit CloseTab(tabName);
  });
}

void SingleJobsite::SummarizeData()
{
  SingleEntry::DoSummarizeWork(data->mwst);
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
