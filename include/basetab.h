#ifndef BASETAB_H
#define BASETAB_H

#include "utils.h"
#include "data_entries.h"
#include "export.h"

#include "QtWidgets\qwidget.h"
#include "QtCore\qsortfilterproxymodel.h"
#include "QtCore\qabstractitemmodel.h"
#include "QtSql\qsqldatabase.h"
#include "QtSql\qsqlquery.h"
#include "QtSql\qsqlquerymodel.h"
#include "QtWidgets\qlineedit.h"
#include "QtPrintSupport\qprinter.h"
#include "QtGui\qtextdocument.h"

#include <vector>
#include <cstdint>

namespace Ui
{
  class basetab;
}

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

class BaseTab : public QWidget
{
  Q_OBJECT
public:
  BaseTab(std::string const &childType, PrintType const &childPrintType, QWidget *parent = nullptr);
  virtual ~BaseTab();

  virtual void SetSettings(Settings *settings);
  virtual void SetDatabase(QSqlDatabase &db);
  virtual Data* GetData(std::string const &artNr);
  virtual std::vector<QString> GetArtNumbers();

public slots:
  virtual void ShowDatabase() = 0;
  virtual void AddEntry() = 0;
  virtual void DeleteEntry() = 0;
  virtual void EditEntry() = 0;
  virtual void FilterList() = 0;
  virtual void ExportToPDF();
  virtual void PrintEntry();
  virtual void SearchEntry();
  virtual void ShowEntry(QModelIndex const &);

protected:
  virtual void EmitToPrinter(QTextDocument &doc);
  void closeEvent(QCloseEvent *event) override;

protected:
  Ui::basetab *m_ui;
  Settings *m_settings;
  bool m_rc;
  Export m_export;
  QPrinter m_pdfPrinter;
  QPrinter m_printer;
  QTextDocument m_doc;
  QSqlQuery m_query;
  QSortFilterProxyModel* m_proxyModel;
  QSqlQueryModel *m_model;
  std::map<std::string, bool> m_tableFilter;
  size_t m_logId;
};

#endif
