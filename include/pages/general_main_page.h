#ifndef GENERAL_MAIN_PAGE_H
#define GENERAL_MAIN_PAGE_H

#include "parent_page.h"
#include "functionality\defines.h"

namespace Ui
{
  class generalMainPage;
}

/**
* @class Page for describing the base of an offer, invoice or jobsite page
*/
class GeneralMainPage : public ParentPage
{
  Q_OBJECT
public:
  /**
  * @brief Public constructor
  * @param settings Settings read by settings file
  * @param invoiceNumber The corresponding invoice number
  * @param parent The parent object
  */
  GeneralMainPage(Settings *settings, 
    std::string const &number, 
    TabName const &childType, 
    QWidget *parent = nullptr);

  /**
  * @brief Public destructor
  */
  ~GeneralMainPage();

  virtual void SetData(GeneralMainData *data);

  public slots:
  /**
  * @brief To be clarified
  */
  void TakeFromAdress();

protected:
  GeneralMainData *m_internalData; ///< internal data

protected:
  Ui::generalMainPage *m_ui;  ///< gui element
  double m_hourlyRate;        ///< hourly rate for this invoice
  QString m_defaultHeadline;  ///< The default headline as defined in settings
  QString m_defaultEndline;   ///< The default endline as defined in settings
};

#endif
