#ifndef GENERAL_MAIN_PAGE_H
#define GENERAL_MAIN_PAGE_H

#include "parent_page.h"
#include "page_framework.h"
#include "functionality\defines.h"

#include "QtSql\qsqlquery.h"

namespace Ui
{
  class generalMainContent;
}

class GeneralMainContent : public ParentPage
{
  Q_OBJECT
public:
  GeneralMainContent(Settings *settings,
    QString const &number, 
    TabName const &childType, 
    QWidget *parent = nullptr);
  ~GeneralMainContent();

  virtual void SetData(GeneralMainData *data);
  void SetFocusToFirst() override;
  void LockNumber();

signals:
  void AddPage();
  void ClosePage();

public slots:
  void TakeFromAdress();

private:
  void CopyData(QString const &key);

public:
  CustomTable *importPage;
  
protected:
  std::unique_ptr<GeneralMainData> m_internalData; ///< internal data

protected:
  Ui::generalMainContent *m_ui;  ///< gui element
  double m_hourlyRate;        ///< hourly rate for this invoice
  QString m_defaultHeadline;  ///< The default headline as defined in settings
  QString m_defaultEndline;   ///< The default endline as defined in settings

private:
  QSqlQuery m_query;       ///< database query
};

#endif
