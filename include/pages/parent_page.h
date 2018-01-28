#ifndef PARENT_PAGE_H
#define PARENT_PAGE_H

#include "functionality\log.h"
#include "functionality\data_entries.h"

#include "QtWidgets\qdialog.h"
#include "QtGui\qevent.h"

class ParentPage : public QDialog
{
  Q_OBJECT
public:
  ParentPage(std::string const &childType, QWidget *parent = nullptr)
    : QDialog(parent)
    , m_logId(Log::GetLog().RegisterInstance(childType))
  {}

  virtual void keyPressEvent(QKeyEvent *ev) override
  {
    if (ev->key() == Qt::Key_Enter || ev->key() == Qt::Key_Return)
      return;
    QDialog::keyPressEvent(ev);
  }

  virtual void SetData(Data *data)
  {
    throw std::runtime_error("Set data not implemented yet for chosen type");
  }

protected:
  size_t m_logId;
};

#endif
