#ifndef UTILS_H
#define UTILS_H

#include "QtWidgets\qdialog.h"
#include "QtWidgets\qdialogbuttonbox.h"
#include "QtWidgets\qlayout.h"
#include "QtWidgets\qcombobox.h"
#include "QtCore\qsortfilterproxymodel.h"
#include "QtWidgets\qmessagebox.h"
#include "QtWidgets\qtextedit.h"
#include "QtGui\qevent.h"
#include "QtWidgets\qtablewidget.h"
#include "QtSql\qsqlquery.h"

#include <vector>

enum TabName
{
  UndefTab,
  MaterialTab,
  ServiceTab,
  AddressTab,
  InvoiceTab,
  JobsiteTab,
  OfferTab,
  PaymentTab
};


namespace german
{
  static std::string ss = "\303\237";
  static std::string Ae = "\303\204";
  static std::string ae = "\303\244";
  static std::string ue = "\303\274";
}

using PartialSumData = std::map<size_t, std::pair<QString, double>>;

namespace util
{
  QMessageBox* GetDeleteMessage(QWidget *parent);
  bool IsDateValid(QString const &txt);
  bool IsNumberValid(QString const &txt);
  QString GetPaddedNumber(QString const &number);
  PartialSumData GetPartialSums(QSqlQuery &query);
}


class Entry : public QDialog
{
public:
  Entry(QWidget *parent);
  virtual ~Entry();

protected:
  QDialogButtonBox *m_buttonBox;
  QVBoxLayout *m_layout;
};


struct SearchFilter : public Entry
{
public:
  SearchFilter(QWidget *parent = nullptr);
  ~SearchFilter();

public:
  QString entry;
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
    std::map<std::string, QString> &mapping,
    QString const &key,
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

protected:
  void keyPressEvent(QKeyEvent *e) override;

signals:
  void Close();

public slots:
  void SetIds(int);

public:
  TabName chosenTab;
  QString chosenId;
  bool importAddress;
  bool importHeadline;
  bool importEndline;
  bool importSubject;

private:
  QComboBox *m_category;
  QTableWidget *m_data;
  size_t m_logInstance;
};


class CustomSortFilterProxyModel : public QSortFilterProxyModel
{
public:
  CustomSortFilterProxyModel(QWidget *parent = nullptr);

protected:
  bool lessThan(QModelIndex const &left, QModelIndex const &right) const override;
};

class ParentPage;

class PageTextEdit : public QTextEdit
{
public:
  PageTextEdit(QWidget *parent = nullptr);

protected:
  void keyPressEvent(QKeyEvent *ev) override;

private:
  ParentPage *m_parent;
};

class CustomTable : public QWidget
{
  Q_OBJECT
public:
  CustomTable(QString const &title, size_t numberRows, QStringList const &columns, QWidget *parent = nullptr);
  void SetColumn(size_t column, std::vector<QString> const &data);
  void SetSortingEnabled(int lastRow = -1);

signals:
  void SetSelected(QString const &);
  void Close();

private:
  QTableWidget *m_table;
};

#endif
