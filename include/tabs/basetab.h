#ifndef BASETAB_H
#define BASETAB_H

#include "functionality/defines.h"
#include "functionality/utils.h"
#include "functionality/data_entries.h"
#include "functionality/export.h"

#include "QtWidgets\qwidget.h"
#include "QtCore\qabstractitemmodel.h"
#include "QtSql\qsqldatabase.h"
#include "QtSql\qsqlquery.h"
#include "QtSql\qsqlquerymodel.h"
#include "QtWidgets\qlineedit.h"
#include "QtPrintSupport\qprinter.h"
#include "QtGui\qtextdocument.h"

#include <vector>
#include <cstdint>
#include <memory>

namespace Ui
{
  class basetab;
}

using TableCols = std::vector<std::pair<std::string, std::string>>;
struct TabData
{
  std::string type;
  std::string tableName;
  QString idString;
  PrintType printType;
  TableCols columns;
  std::vector<std::string> defaultSelection;
};


class BaseTab : public QWidget
{
  Q_OBJECT
public:
  BaseTab(TabData const &childData, QWidget *parent = nullptr);
  virtual ~BaseTab();

  virtual void SetSettings(Settings *settings);
  virtual void SetDatabase(QSqlDatabase &db);
  virtual std::unique_ptr<Data> GetData(std::string const &artNr);
  virtual void SetData(std::unique_ptr<Data> &data);
  virtual std::vector<QString> GetArtNumbers();
  virtual void ShowDatabase();

public slots:
  virtual void AddEntry() = 0;
  virtual void DeleteEntry() = 0;
  virtual void EditEntry() = 0;
  virtual void FilterList();
  virtual void ExportToPDF();
  virtual void PrintEntry();
  virtual void SearchEntry();
  virtual void ShowEntry(QModelIndex const &);

protected:
  virtual ReturnValue PrepareDoc(bool withLogo);
  virtual void EmitToPrinter(QTextDocument &doc);

protected:
  Ui::basetab *m_ui;
  Settings *m_settings;
  bool m_rc;
  Export m_export;
  QPrinter m_pdfPrinter;
  QPrinter m_printer;
  TabData m_data;
  QTextDocument m_doc;
  QSqlQuery m_query;
  CustomSortFilterProxyModel* m_proxyModel;
  QSqlQueryModel *m_model;
  std::map<std::string, bool> m_tableFilter;
  size_t m_logId;
};

#endif
