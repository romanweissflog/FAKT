#ifndef PAGE_FRAMEWORK_H
#define PAGE_FRAMEWORK_H

#include "QtWidgets\qwidget.h"

namespace Ui
{
  class pageFramework;
}

class PageFramework : public QWidget
{
  Q_OBJECT
public:
  PageFramework(QWidget *parent = nullptr);
  ~PageFramework();

protected:
  void keyPressEvent(QKeyEvent *ev) override;

signals:
  void Accepted();
  void Declined();

protected:
  Ui::pageFramework *m_ui;
};

#endif
