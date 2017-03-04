#ifndef UTILS_H
#define UTILS_H

#include "QtWidgets\qdialog.h"
#include "QtWidgets\qdialogbuttonbox.h"
#include "QtWidgets\qlayout.h"

#include <vector>

class Entry : public QDialog
{
public:
  Entry(QWidget *parent);
  virtual ~Entry();

protected:
  QDialogButtonBox *m_buttonBox;
  std::vector<QWidget*> m_widgets;
  QVBoxLayout *m_layout;
};

class EditOneEntry : public Entry
{
public:
  EditOneEntry(QString oldValue, QWidget *parent = nullptr);
  virtual ~EditOneEntry();

public:
  QString newValue;
};

#endif
