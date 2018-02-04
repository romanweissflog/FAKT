#ifndef GENERAL_PAGE_H
#define GENERAL_PAGE_H

#include "parent_page.h"

#include "QtSql\qsqlquery.h"

namespace Ui
{
  class generalPage;
}
class GeneralPage : public ParentPage
{
  Q_OBJECT
public:
  GeneralPage(Settings *settings,
    uint64_t number,
    std::string const &child,
    QSqlQuery &query,
    QWidget *parent = nullptr);

  void CopyData(GeneralData *data);

  ~GeneralPage();

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
  Ui::generalPage *m_ui;  ///< gui element
  QSqlQuery &m_query;     ///< database query
  double m_hourlyRate;    ///< hourly rate used for this piece
};

#endif
