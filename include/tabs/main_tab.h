#ifndef MAIN_TAB_H
#define MAIN_TAB_H

#include "QtWidgets\qwidget.h"

namespace Ui
{
  class main_tab;
}

class MainTab : public QWidget
{
  Q_OBJECT
public:
  MainTab(QWidget *parent = nullptr);
  ~MainTab();

signals:
  void AddTab(int);

private:
  Ui::main_tab *m_ui;
};

#endif
