#ifndef GENERAL_PAGE_H
#define GENERAL_PAGE_H

#include "parent_page.h"
#include "page_framework.h"

#include "QtSql\qsqlquery.h"

namespace Ui
{
  class generalContent;
}
class GeneralContent : public ParentPage
{
  Q_OBJECT
public:
  GeneralContent(Settings *settings,
    uint64_t number,
    std::string const &child,
    QSqlQuery &query,
    QWidget *parent = nullptr);

  void CopyData(GeneralData *data);

  ~GeneralContent();

  void SetFocusToFirst() override;

public slots:
  void TakeFromMaterial();
  void TakeFromService();
  void MakeNewEntry();

private:
  void Calculate();
  void SetConnections();

public:
  GeneralData data;       /// internal data

private:
  Ui::generalContent *m_ui;  ///< gui element
  QSqlQuery &m_query;     ///< database query
  double m_hourlyRate;    ///< hourly rate used for this piece
};


class GeneralPage : public PageFramework
{
  Q_OBJECT
public:
  GeneralPage(Settings *settings,
    uint64_t number,
    std::string const &child,
    QSqlQuery &query,
    QWidget *parent = nullptr);
  ~GeneralPage();

public:
  GeneralContent * content;
};

#endif
