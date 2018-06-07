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
  DatabaseData entries;
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
  virtual DatabaseData GetData(std::string const &key);
  virtual void SetData(DatabaseData const &data);
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
  virtual void DeleteDataTable(QString const &id);
  virtual QSqlQuery PrepareGroupQuery(QString const &sql, QSqlDatabase const &db);
  virtual QSqlQuery PrepareExtraQuery(QString const &type, std::string const &number);
  virtual void DeleteData(QString const &key);
  virtual void AddData(DatabaseData const &data);
  virtual void EditData(QString const &key, DatabaseData const &data);

protected:
  Ui::basetab *m_ui;
  Settings *m_settings;
  bool m_rc;
  Export m_export;
  TabData m_data;
  QSqlQuery m_query;
  CustomSortFilterProxyModel* m_proxyModel;
  QSqlQueryModel *m_model;
  std::map<QString, bool> m_tableFilter;
  size_t m_logId;
  std::map<Qt::Key, QShortcut*> m_shortCuts;
};

#endif
