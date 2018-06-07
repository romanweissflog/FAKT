#ifndef PAGE_FRAMEWORK_H
#define PAGE_FRAMEWORK_H

#include "functionality\data_entries.h"

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

  virtual DatabaseData GetData() const
  {
    return{};
  }

protected:
  void keyPressEvent(QKeyEvent *ev) override;

  virtual void HandleBeforeAccept()
  {}

signals:
  void Accepted();
  void Declined();
  void AddExtraPage(QWidget *, QString const &);
  void CloseExtraPage(QString const &);

protected:
  Ui::pageFramework *m_ui;
};

#endif
