#ifndef MATERIAL_H
#define MATERIAL_H

#include "utils.h"
#include "basetab.h"

struct MaterialEntryData
{
  QString artNumber;
  QString artDescr;
  QString unit;
  double ep;
};

class MaterialEditEntry : public Entry
{
public:
  MaterialEditEntry(QString oldValue, QWidget *parent = nullptr);
  virtual ~MaterialEditEntry();

public:
  QLineEdit *newValue;
};

class MaterialDeleteEntry : public Entry
{
public:
  MaterialDeleteEntry(QWidget *parent = nullptr);
  virtual ~MaterialDeleteEntry();

public:
  QLineEdit *idToBeDeleted;
};

class MaterialEntry : public Entry
{
  Q_OBJECT
public:
  MaterialEntry(QWidget *parent = nullptr);
  virtual ~MaterialEntry();

public:
  MaterialEntryData data;
};

class Material : public BaseTab
{
  Q_OBJECT
public:
  Material(QWidget *parent = nullptr);
  ~Material();
  
  void SetDatabase(QSqlDatabase &db);

public slots:
  void ShowDatabase() override;
  void AddEntry() override;
  void DeleteEntry() override;
  void EditEntry(const QModelIndex &) override;
  void SearchEntry() override;
  void FilterList() override;
  void OrganizeList() override;
};

#endif
