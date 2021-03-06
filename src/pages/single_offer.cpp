#include "pages\single_offer.h"
#include "functionality\overwatch.h"
#include "pages\offer_page.h"


SingleOffer::SingleOffer(size_t number, QWidget *parent)
  : SingleEntry(number, "A", TabName::OfferTab, parent)
  , data(static_cast<OfferData*>(m_internalData.get()))
{
  m_data.tabName = "Angebote";
  this->setWindowTitle("Angebot");
}

void SingleOffer::Calculate()
{
  data->total = data->materialTotal + data->helperTotal + data->serviceTotal;
  data->mwstTotal = util::Precision2Round(data->total / 100 * m_settings->mwst);
  data->brutto = data->total + data->mwstTotal;
}

void SingleOffer::Recalculate(Data *edited)
{
  OfferData *editedData = reinterpret_cast<OfferData*>(edited);
  if (util::IsDevisionByZero(data->hourlyRate))
  {
    throw std::runtime_error("Devision by zero detected");
  }
  double time = util::Precision2Round(60.0 * data->serviceTotal / data->hourlyRate);
  data->serviceTotal = util::Precision2Round(time / 60.0 * editedData->hourlyRate);
  data->total = data->serviceTotal + data->helperTotal + data->materialTotal;
  data->mwstTotal = util::Precision2Round(data->total / 100 * m_settings->mwst);
  data->brutto = data->total + data->mwstTotal;
  SingleEntry::Recalculate(edited);
}

void SingleOffer::EditAfterImport(ImportWidget *importWidget)
{
  SingleEntry::EditAfterImport(importWidget);
  data->mwstTotal = util::Precision2Round(data->total / 100 * m_settings->mwst);
  data->brutto = data->total + data->mwstTotal;
}

void SingleOffer::EditMeta()
{
  QString number = QString::number(m_number);
  auto tab = Overwatch::GetInstance().GetTabPointer(TabName::OfferTab);
  OfferPage *editPage = new OfferPage(m_settings, number);
  connect(editPage, &PageFramework::AddExtraPage, [this, editPage, number](QWidget *widget, QString const &txt)
  {
    emit AddSubtab(widget, "Angebote:" + number + ":Allgemein:" + txt);
  });
  connect(editPage, &PageFramework::CloseExtraPage, [this, editPage, number](QString const &txt)
  {
    emit CloseTab("Angebote:" + number + ":Allgemein:" + txt);
  });

  std::unique_ptr<OfferData> data(static_cast<OfferData*>(tab->GetData(number.toStdString()).release()));
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

void SingleOffer::SummarizeData()
{
  SingleEntry::DoSummarizeWork(m_settings->mwst);
}

void SingleOffer::SetLastData(Data *input)
{
  SingleEntry::SetLastData(input);
  OfferData *offerData = static_cast<OfferData*>(input);
  data->deadLine = offerData->deadLine;
}
