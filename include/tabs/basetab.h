#ifndef BASETAB_H
#define BASETAB_H

#include "functionality/defines.h"
#include "functionality/utils.h"
#include "functionality/data_entries.h"
#include "functionality/export.h"
#include "pages/general_page.h"

#include "QtWidgets\qwidget.h"
#include "QtCore\qabstractitemmodel.h"
#include "QtSql\qsqldatabase.h"
#include "QtSql\qsqlquery.h"
#include "QtSql\qsqlquerymodel.h"
#include "QtWidgets\qlineedit.h"
#include "QtWidgets\qshortcut.h"

#include <vector>
#include <cstdint>
#include <memory>

namespace Ui
{
  class basetab;
}

using TableCols = std::vector<std::pair<std::string, QString>>;
struct TabData
{
  TabName tabType;
  std::string type;
  std::string tableName;
  QString dataTableSuffix;
  QString dataDatabase;
  QString tabName;
  QString idString;
  uint16_t printMask;
  TableCols columns;
  std::vector<std::string> defaultSelection;
};


class Payment;
class SingleEntry;

class BaseTab : public QWidget
{
  Q_OBJECT
public:
  BaseTab(TabData const &childData, QWidget *parent = nullptr);
  virtual ~BaseTab();

  virtual void SetSettings(Settings *settings);
  virtual void SetDatabase(QSqlDatabase &db);
  virtual std::unique_ptr<Data> GetData(std::string const &artNr);
  virtual void SetData(Data *data);
  virtual std::map<QString, std::vector<QString>> GetRowData(std::vector<QString> const &columns);
  virtual void ShowDatabase();

signals:
  void CloseTab(QString const &);
  void AddSubtab(QWidget *, QString const &);
  void AddSubtab(SingleEntry *, QString const &);
  void AddSubtab(Payment *, QString const &);

public slots:
  virtual void AddEntry();
  virtual void DeleteEntry();
  virtual void EditEntry();
  virtual void FilterList();
  virtual void ExportToPDF();
  virtual void PrintEntry();
  virtual void SearchEntry();
  virtual void EditEntryAfterClick(QModelIndex const &);
  virtual void OnEscape();

protected:
  virtual ReturnValue PrepareDoc(bool withLogo);
  virtual QSqlQuery PrepareGroupQuery(QString const &sql, QSqlDatabase const &db);
  virtual QSqlQuery PrepareExtraQuery(QString const &type, std::string const &number);
  virtual void DeleteData(QString const &key);

protected:
  Ui::basetab *m_ui;
  Settings *m_settings;
  bool m_rc;
  Export m_export;
  TabData m_data;
  QSqlQuery m_query;
  CustomSortFilterProxyModel* m_proxyModel;
  QSqlQueryModel *m_model;
  std::map<std::string, bool> m_tableFilter;
  size_t m_logId;
  std::map<Qt::Key, QShortcut*> m_shortCuts;
};

#endif
