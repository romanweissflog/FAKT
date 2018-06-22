#ifndef SUMMARY_PAGE_H
#define SUMMARY_PAGE_H

#include "functionality\data_entries.h"
#include "functionality\utils.h"
#include "pages/page_framework.h"
#include "pages/parent_page.h"

#include "QtWidgets\qwidget.h"
#include "QtSql\qsqlquery.h"
#include "QtCore\qstring.h"

namespace Ui
{
  class summaryContent;
}

class SummaryContent : public ParentPage
{
  Q_OBJECT
public:
  SummaryContent(GeneralMainData const &data, 
    QSqlQuery &query,
    QString const &table, 
    double mwst, 
    QWidget *parent = nullptr);

  void SetFocusToFirst() override;

signals:
  void AddPage();
  void ClosePage();

public slots:
  void PartialSums();
  void CorrectData();

private:
  void SetMainData(GeneralMainData const &data);
  void CalculateDetailData(double hourlyRate);

public:
  CustomTable *partialSums;
  GeneralMainData correctedData;

private:
  Ui::summaryContent *m_ui;
  size_t m_logId;
  QSqlQuery &m_query;
  QString m_table;
  double m_mwst;
};

class SummaryPage : public PageFramework
{
  Q_OBJECT
public:
  SummaryPage(GeneralMainData const &data, 
    QSqlQuery &query, 
    QString const &table, 
    double mwst, 
    QWidget *parent = nullptr);

public:
  SummaryContent *content;
};

#endif
