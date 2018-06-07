#ifndef ADDRESS_PAGE_H
#define ADDRESS_PAGE_H

#include "parent_page.h"
#include "page_framework.h"

#include "QtSql\qsqlquery.h"

#include <optional>

namespace Ui
{
  class addressContent;
}

class AddressContent : public ParentPage
{
  Q_OBJECT
public:
  AddressContent(Settings *settings, QString const &edit = "", QWidget *parent = nullptr);
  ~AddressContent();

  void SetFocusToFirst() override;

signals:
  void AddPage();
  void ClosePage();

public slots:
  void CopyData(QString);
  void Copy();

public:
  CustomTable *importPage;
  AddressData data;
  std::optional<QString> numberForSettings;

private:
  Ui::addressContent *m_ui; 
  QSqlQuery m_query;       
};


class AddressPage : public PageFramework
{
  Q_OBJECT
public:
  AddressPage(Settings *settings, QString const &edit = "", QWidget *parent = nullptr);

private:
  void HandleBeforeAccept() override;

public:
  AddressContent *content;

private:
  Settings *m_settings;
};

#endif
