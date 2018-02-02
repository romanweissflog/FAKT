#ifndef SUMMARY_PAGE_H
#define SUMMARY_PAGE_H

#include "functionality\data_entries.h"

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
  SummaryPage(GeneralMainData const &data, QSqlQuery &query, QString const &table, QWidget *parent = nullptr);
  ~SummaryPage();

signals:
  void Close();

public slots:
  void CalculateGroups();

private:
  void SetMainData(GeneralMainData const &data);
  void CalculateDetailData(double hourlyRate);

private:
  Ui::summaryPage *m_ui;
  QSqlQuery &m_query;
  QString const &m_table;
};

#endif
