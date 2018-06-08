#ifndef SUMMARY_PAGE_H
#define SUMMARY_PAGE_H

#include "functionality\data_entries.h"
#include "functionality\utils.h"

#include "QtWidgets\qwidget.h"
#include "QtSql\qsqlquery.h"
#include "QtCore\qstring.h"

namespace Ui
{
  class summaryPage;
}

class SummaryPage : public QWidget
{
  Q_OBJECT
public:
  SummaryPage(DatabaseData const &data, QSqlQuery &query, QString const &table, QWidget *parent = nullptr);
  ~SummaryPage();

signals:
  void Close();
  void AddPartialSums();
  void ClosePartialSums();

public slots:
  void PartialSums();

private:
  void SetMainData(GeneralMainData const &data);
  void CalculateDetailData(double hourlyRate);

public:
  CustomTable *partialSums;

private:
  Ui::summaryPage *m_ui;
  size_t m_logId;
  QSqlQuery &m_query;
  QString m_table;
};

#endif
