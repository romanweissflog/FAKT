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
  ParentPage(std::string const &childType, QWidget *parent = nullptr);

  virtual void keyPressEvent(QKeyEvent *ev) override;

  virtual void SetData(Data *data);

  virtual void SetFocusToFirst() = 0;

protected:
  size_t m_logId;
};

#endif
