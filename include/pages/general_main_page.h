#ifndef GENERAL_MAIN_PAGE_H
#define GENERAL_MAIN_PAGE_H

#include "parent_page.h"
#include "functionality\defines.h"

namespace Ui
{
  class generalMainPage;
}

class GeneralMainPage : public ParentPage
{
  Q_OBJECT
public:
  GeneralMainPage(Settings *settings, 
    QString const &number, 
    TabName const &childType, 
    QWidget *parent = nullptr);
  ~GeneralMainPage();

  virtual void SetData(GeneralMainData *data);

public slots:
  void TakeFromAdress();

protected:
  std::unique_ptr<GeneralMainData> m_internalData; ///< internal data

protected:
  Ui::generalMainPage *m_ui;  ///< gui element
  double m_hourlyRate;        ///< hourly rate for this invoice
  QString m_defaultHeadline;  ///< The default headline as defined in settings
  QString m_defaultEndline;   ///< The default endline as defined in settings
};

#endif
