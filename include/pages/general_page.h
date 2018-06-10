#ifndef GENERAL_PAGE_H
#define GENERAL_PAGE_H

#include "parent_page.h"
#include "page_framework.h"

#include "QtSql\qsqlquery.h"

#include <optional>

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
    std::optional<double> const &hourlyRate,
    QString const &key = {},
    QWidget *parent = nullptr);
  
  ~GeneralContent();

  void CopyData(GeneralData *data);
  void SetFocusToFirst() override;

signals:
  void AddPage();
  void ClosePage();

public slots:
  void TakeFromMaterial();
  void TakeFromService();

private:
  void Calculate();
  void SetConnections();
  void CopyMaterialData(QString const &key);
  void CopyServiceData(QString const &key);

public:
  GeneralData data;       /// internal data
  CustomTable *importPage;

private:
  Ui::generalContent *m_ui;  ///< gui element
  QSqlQuery m_query;     ///< database query
  double m_hourlyRate;    ///< hourly rate used for this piece
};


class GeneralPage : public PageFramework
{
  Q_OBJECT
public:
  GeneralPage(Settings *settings,
    uint64_t number,
    std::string const &child,
    std::optional<double> const &hourlyRate,
    QString const &key = {},
    QWidget *parent = nullptr);
  ~GeneralPage();

public:
  GeneralContent * content;
};

#endif
