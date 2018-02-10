#include "functionality\record.h"

#include "functionality\log.h"

#include "QtCore\qvariant.h"
#include "QtSql\qsqlerror.h"
#include "QtCore\qdebug.h"

RecordTable::RecordTable(InputData const &input, RecordData &data, QSqlQuery &query)
  : m_query(query)
  , m_data(data)
  , m_logId(Log::GetLog().RegisterInstance("RecordTable"))
  , m_inputData(input)
{
}

ReturnValue RecordTable::GetData(QString const &key)
{
  QString sql = QString("SELECT * FROM ") + m_inputData.tableName + " WHERE " + m_inputData.idString + " = :ID";
  int rc = m_query.prepare(sql);
  if (!rc)
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, m_query.lastError().text().toStdString());
    return ReturnValue::ReturnFailure;
  }
  m_query.bindValue(":ID", key);
  rc = m_query.exec();
  if (!rc)
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, m_query.lastError().text().toStdString());
    return ReturnValue::ReturnFailure;
  }
  rc = m_query.next();
  if (!rc)
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, m_query.lastError().text().toStdString());
    return ReturnValue::ReturnFailure;
  }

  try
  {
    for (auto &&e : m_data)
    {
      auto idx = e.second.which();
      switch (idx)
      {
      case 0: e.second = m_query.value(e.first).value<uint32_t>(); break;
      case 1: e.second = m_query.value(e.first).value<double>(); break;
      case 2: e.second = m_query.value(e.first).value<QString>(); break;
      default:
        Log::GetLog().Write(LogType::LogTypeError, m_logId, "Unknown variant type");
        return ReturnValue::ReturnFailure;
      }
    }
  }
  catch (std::exception e)
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, "Unknown exception");
    return ReturnValue::ReturnFailure;
  }
  return ReturnValue::ReturnSuccess;
}

ReturnValue RecordTable::AddData()
{
  QString sql = QString("INSERT INTO ") + m_inputData.tableName + " (";
  for (auto &&it = m_data.begin(); it != m_data.end(); ++it)
  {
    sql += it->first;
    if (it != std::prev(m_data.end()))
    {
      sql += ", ";
    }
  }
  sql += ") VALUES (";
  for (auto &&it = m_data.begin(); it != m_data.end(); ++it)
  {
    auto &&v = it->second;
    auto &&idx = v.which();
    switch (idx)
    {
    case 0: sql += QString::number(boost::get<uint32_t>(v));  break;
    case 1: sql += QString::number(boost::get<double>(v)); break;
    case 2: sql += "'" + boost::get<QString>(v) + "'"; break;
    default:
      Log::GetLog().Write(LogType::LogTypeError, m_logId, "Unknown variant type");
      return ReturnValue::ReturnFailure;
    }
    if (it != std::prev(m_data.end()))
    {
      sql += ", ";
    }
  }
  sql += ")";
  qDebug() << sql;
  auto rc = m_query.exec(sql);
  if (!rc)
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, m_query.lastError().text().toStdString());
    return ReturnValue::ReturnFailure;
  }
  return ReturnValue::ReturnSuccess;
}


ReturnValue RecordTable::EditData(QString const &key)
{
  QString sql = QString("UPDATE ") + m_inputData.tableName + " SET ";
  for (auto &&it = m_data.begin(); it != m_data.end(); ++it)
  {
    auto &&v = it->second;
    auto &&idx = v.which();
    sql += it->first + " = ";
    switch (idx)
    {
    case 0: sql += QString::number(boost::get<uint32_t>(v)); break;
    case 1: sql += QString::number(boost::get<double>(v)); break;
    case 2: sql += "'" + boost::get<QString>(v) + "'"; break;
    default:
      Log::GetLog().Write(LogType::LogTypeError, m_logId, "Unknown variant type");
      return ReturnValue::ReturnFailure;
    }
    sql += " ";
  }
  sql += "WHERE " + m_inputData.idString + " = " + key;
  auto rc = m_query.exec(sql);
  if (!rc)
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, m_query.lastError().text().toStdString());
    return ReturnValue::ReturnFailure;
  }
  return ReturnValue::ReturnSuccess;
}

ReturnValue RecordTable::DeleteData(QString const &key)
{
  QString const sql = "DELETE FROM " + m_inputData.tableName + " WHERE " + m_inputData.idString + " = :ID";
  auto rc = m_query.prepare(sql);
  if (!rc)
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, m_query.lastError().text().toStdString());
    return ReturnValue::ReturnFailure;
  }
  m_query.bindValue(":ID", key);
  rc = m_query.exec();
  if (!rc)
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, m_query.lastError().text().toStdString());
    return ReturnValue::ReturnFailure;
  }
  return ReturnValue::ReturnSuccess;
}
