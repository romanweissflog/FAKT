#ifndef ADDING_PAGES_H
#define ADDING_PAGES_H

#include "data_entries.h"

#include "QtWidgets\qwidget.h"
#include "QtWidgets\qdialog.h"

namespace Ui
{
  class leistungPage;
}

class LeistungPage : public QDialog
{
  Q_OBJECT
public:
  LeistungPage(Settings &settings, QWidget *parent = nullptr);
  ~LeistungPage();

private:
  void Calculate();

public slots:
  void CopyData();

public:
  LeistungData data;

private:
  Ui::leistungPage *m_ui;
  double m_euroPerMin;
};

#endif