#ifndef UTILS_H
#define UTILS_H

#include "QtWidgets\qdialog.h"
#include "QtWidgets\qdialogbuttonbox.h"
#include "QtWidgets\qlayout.h"
#include "QtWidgets\qcombobox.h"

#include <vector>

enum TabName
{
  MaterialTab,
  ServiceTab,
  AddressTab,
  InvoiceTab,
  JobsiteTab,
  OfferTab
};


class Entry : public QDialog
{
public:
  Entry(QWidget *parent);
  virtual ~Entry();

protected:
  QDialogButtonBox *m_buttonBox;
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

struct SearchFilter : public Entry
{
public:
  SearchFilter(QWidget *parent = nullptr);
  ~SearchFilter();

public:
  QString entry;
};


struct ShowValue : public Entry
{
public:
  ShowValue(QString value, QWidget *parent = nullptr);
  ~ShowValue();
};


struct ShowValueList : public Entry
{
public:
  ShowValueList(std::vector<QString> const &list, QWidget *parent = nullptr);
  ~ShowValueList();

public:
  QString currentItem;
};


class FilterTable : public Entry
{
public:
  FilterTable(std::map<std::string, bool> &oldFilter,
    std::map<std::string, std::string> &mapping,
    QWidget *parent = nullptr);
  virtual ~FilterTable();

public:
  QString newValue;
};


class ImportWidget : public Entry
{
  Q_OBJECT
public:
  ImportWidget(QWidget *parent = nullptr);
  ~ImportWidget();

public slots:
  void SetIds(int);

public:
  TabName chosenTab;
  std::string chosenId;
  bool importAddress;
  bool importHeadline;
  bool importEndline;
  bool importSubject;

private:
  QComboBox *m_category;
  QComboBox *m_ids;
};

#endif
