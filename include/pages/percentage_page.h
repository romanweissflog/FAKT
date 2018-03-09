#ifndef PERCENTAGE_PAGE_H
#define PERCENTAGE_PAGE_H

#include "functionality\data_entries.h"
#include "page_framework.h"

#include "QtWidgets\qwidget.h"
#include "QtSql\qsqlquery.h"

namespace Ui
{
  class percentageContent;
}

class PercentageContent : public QWidget
{
  Q_OBJECT
public:
  PercentageContent(Settings *settings, GeneralMainData const &data, QWidget *parent = nullptr);
  ~PercentageContent();

  void SetFocusToFirst();

private:
  void Calculate();

public:
  GeneralMainData data;
  double percentageMaterial;
  double percentageService;

private:
  Ui::percentageContent *m_ui;
  double m_mwst;
  double m_inputMaterial;
  double m_inputService;
};

class PercentagePage : public PageFramework
{
public:
  PercentagePage(Settings *settings, GeneralMainData const &data, QWidget *parent = nullptr);
  ~PercentagePage();

public:
  PercentageContent *content;
};

#endif
