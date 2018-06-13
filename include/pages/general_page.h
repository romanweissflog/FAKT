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
  std::optional<QString> lastMaterialImport;
  std::optional<QString> lastServiceImport;

private:
  Ui::generalContent *m_ui;
  QSqlQuery m_query;
  double m_hourlyRate;  
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

  std::optional<QString> GetLastMaterialImportKey() const;
  std::optional<QString> GetLastServiceImportKey() const;
  void SetLastMaterialImportKey(std::optional<QString> const &key);
  void SetLastServiceImportKey(std::optional<QString> const &key);

public:
  GeneralContent *content;
};

#endif
