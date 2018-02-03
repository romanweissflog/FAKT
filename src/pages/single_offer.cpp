#include "pages\single_offer.h"
#include "functionality\overwatch.h"
#include "pages\offer_page.h"


SingleOffer::SingleOffer(size_t number, std::string const &tableName, QWidget *parent)
  : SingleEntry(number, tableName, TabName::OfferTab, parent)
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
  data->skonto = data->brutto / 100 * data->skonto + data->brutto;
}

void SingleOffer::Recalculate(Data *edited)
{
  OfferData *editedData = reinterpret_cast<OfferData*>(edited);
  data->mwstTotal = data->total / 100 * m_settings->mwst;
  data->brutto = data->total + data->mwstTotal;
  data->skonto = data->brutto / 100 * editedData->skonto + data->brutto;
  SingleEntry::Recalculate(edited);
}

void SingleOffer::EditMeta()
{
  QString number = QString::number(m_number);
  auto tab = Overwatch::GetInstance().GetTabPointer(TabName::OfferTab);
  OfferPage *editPage = new OfferPage(m_settings, number);
  std::unique_ptr<OfferData> data(static_cast<OfferData*>(tab->GetData(number.toStdString()).release()));
  editPage->SetData(data.get());
  
  QString const tabName = m_data.tabName + ":" + QString::number(m_number) + ":Allgemein";
  editPage->hide();
  AddSubtab(editPage, tabName);
  editPage->setFocus();
  if (editPage->exec() == QDialog::Accepted)
  {
    Recalculate(editPage->data);
    tab->SetData(editPage->data);
  }
  CloseTab(tabName);
}
