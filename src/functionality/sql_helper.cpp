#include "functionality\sql_helper.h"

void GenerateInsertCommandInternal(std::string &skeleton, std::pair<QString, SingleData> const &current)
{
  auto keyPos = skeleton.find(") VALUES (");
  skeleton.insert(keyPos, current.first.toStdString() + ", ");
  auto valPos = skeleton.find_last_of(")");
  skeleton.insert(valPos, current.second.entry.toString().toStdString() + ", ");
}

QString GenerateEditCommandInternal(std::pair<QString, SingleData> const &d)
{
  return d.first + " = " + d.second.entry.toString() + ", ";
}

QString GenerateInsertCommand(std::string const &table, DatabaseDataEntry::const_iterator begin, DatabaseDataEntry::const_iterator end)
{
  std::string skeleton = "() VALUES ()";
  std::string sql = "INSERT INTO " + table + " ";
  for (; begin != std::prev(end); ++begin)
  {
    GenerateInsertCommandInternal(skeleton, *begin);
  }
  return QString::fromStdString(sql + skeleton);
}

QString GenerateEditCommand(std::string const &table,
  QString const &keyName,
  QString const &key,
  DatabaseDataEntry::const_iterator begin, DatabaseDataEntry::const_iterator end)
{
  QString sql = "UPDATE " + QString::fromStdString(table) + " SET ";
  for (; begin != std::prev(end); ++begin)
  {
    sql += GenerateEditCommandInternal(*begin);
  }
  sql += begin->first + " = " + begin->second.entry.toString();
  sql += " WHERE " + keyName + " = '" + key + "'";
  return sql;
}


QString GenerateSelectAllCommand(std::string const &table,
  DatabaseDataEntry::const_iterator begin, DatabaseDataEntry::const_iterator end)
{
  QString sql = "SELECT ";
  for (; begin != std::prev(end); ++begin)
  {
    sql += begin->first + ", ";
  }
  sql += std::prev(end)->first + " FROM " + QString::fromStdString(table);
  return sql;
}

QString GenerateSelectRowCommand(std::string const &table,
  QString const &key, 
  QString const &entry,
  DatabaseDataEntry::const_iterator begin, DatabaseDataEntry::const_iterator end)
{
  QString sql = "SELECT ";
  for (; begin != std::prev(end); ++begin)
  {
    sql += begin->first + ", ";
  }
  sql += std::prev(end)->first + " FROM " + QString::fromStdString(table);
  sql += " WHERE " + key + " = '" + entry + "'";
  return sql;
}