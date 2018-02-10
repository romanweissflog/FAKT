#ifndef TABLE_H
#define TABLE_H

#include "defines.h"

#include "boost/variant.hpp"

#include "QtCore\qstring.h"
#include "QtSql\qsqlquery.h"

#include <string>
#include <cstdint>
#include <map>

using RecordData = std::map<QString, boost::variant<uint32_t, double, QString>>;

struct InputData
{
  QString tableName;
  QString idString;
};

struct RecordTable
{
public:
  RecordTable(InputData const &input, RecordData &data, QSqlQuery &query);

  ReturnValue GetData(QString const &key);
  ReturnValue AddData();
  ReturnValue EditData(QString const &key);
  ReturnValue DeleteData(QString const &key);

private:
  QSqlQuery &m_query;
  RecordData &m_data;
  size_t m_logId;
  InputData m_inputData;
};

#endif
