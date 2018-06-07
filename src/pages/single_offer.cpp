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
  data->mwstTotal = data->total / 100 * m_settings->mwst;
  data->brutto = data->total + data->mwstTotal;
}

void SingleOffer::Recalculate(Data *edited)
{
  OfferData *editedData = reinterpret_cast<OfferData*>(edited);
  data->mwstTotal = data->total / 100 * m_settings->mwst;
  data->brutto = data->total + data->mwstTotal;
  SingleEntry::Recalculate(edited);
}

void SingleOffer::EditMeta()
{
  //QString number = QString::number(m_number);
  //auto tab = Overwatch::GetInstance().GetTabPointer(TabName::OfferTab);
  //OfferPage *editPage = new OfferPage(m_settings, number);
  //connect(editPage, &PageFramework::AddExtraPage, [this, editPage, number](QWidget *widget, QString const &txt)
  //{
  //  emit AddSubtab(widget, "Angebote:" + number + ":Allgemein:" + txt);
  //});
  //connect(editPage, &PageFramework::CloseExtraPage, [this, editPage, number](QString const &txt)
  //{
  //  emit CloseTab("Angebote:" + number + ":Allgemein:" + txt);
  //});

  //std::unique_ptr<OfferData> data(static_cast<OfferData*>(tab->GetData(number.toStdString()).release()));
  //if (!data)
  //{
  //  return;
  //}
  //editPage->content->SetData(data.get());
  //editPage->content->LockNumber();
  //
  //QString const tabName = m_data.tabName + ":" + QString::number(m_number) + ":Allgemein";
  //AddSubtab(editPage, tabName);
  //connect(editPage, &PageFramework::Accepted, [this, tab, editPage, tabName]()
  //{
  //  Recalculate(editPage->content->data);
  //  tab->SetData(editPage->content->data);
  //  emit CloseTab(tabName);
  //});
  //connect(editPage, &PageFramework::Declined, [this, tabName]()
  //{
  //  emit CloseTab(tabName);
  //});
}

void SingleOffer::SetLastData(Data *input)
{
  SingleEntry::SetLastData(input);
  OfferData *offerData = static_cast<OfferData*>(input);
  data->deadLine = offerData->deadLine;
}
