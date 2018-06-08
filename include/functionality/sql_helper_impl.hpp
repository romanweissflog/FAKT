#ifndef SQL_HELPER_IMPL_HPP_
#define SQL_HELPER_IMPL_HPP_

inline std::string GenerateInsertCommandInternal(std::string &skeleton, SqlPair const &current)
{
  auto keyPos = skeleton.find(") VALUES (");
  skeleton.insert(keyPos, current.key);
  auto valPos = skeleton.find_last_of(")");
  skeleton.insert(valPos, current.value);
  return skeleton;
}

template<typename ...Args>
inline std::string GenerateInsertCommandInternal(std::string &skeleton, SqlPair const &current, Args... args)
{
  auto keyPos = skeleton.find(") VALUES (");
  skeleton.insert(keyPos, current.key + ", ");
  auto valPos = skeleton.find_last_of(")");
  skeleton.insert(valPos, current.value + ", ");
  return GenerateInsertCommandInternal(skeleton, args...);
}

inline void GenerateInsertCommandInternal(std::string &skeleton, std::pair<QString, SingleData> const &current)
{
  auto keyPos = skeleton.find(") VALUES (");
  skeleton.insert(keyPos, current.first.toStdString() + ", ");
  auto valPos = skeleton.find_last_of(")");
  skeleton.insert(valPos, current.second.entry.toString().toStdString() + ", ");
}

inline std::string GenerateEditCommandInternal(SqlPair const &current)
{
  return current.key + " = " + current.value;
}

template<typename ...Args>
inline std::string GenerateEditCommandInternal(SqlPair const &current, Args... args)
{
  return current.key + " = " + current.value + ", " + GenerateEditCommandInternal(args...);
}

inline QString GenerateEditCommandInternal(std::pair<QString, SingleData> const &d)
{
  return d.first + " = " + d.second.entry.toString() + ", ";
}

template<typename ...Args>
std::string GenerateInsertCommand(std::string const &table, Args... args)
{
  std::string skeleton = "() VALUES ()";
  return "INSERT INTO " + table + " " + GenerateInsertCommandInternal(skeleton, args...);
}

inline std::string GenerateInsertCommand(std::string const &table, DatabaseDataEntry::const_iterator begin, DatabaseDataEntry::const_iterator end)
{
  std::string skeleton = "() VALUES ()";
  std::string sql = "INSERT INTO " + table + " ";
  for (; begin != std::prev(end); ++begin)
  {
    GenerateInsertCommandInternal(skeleton, *begin);
  }
  return sql + skeleton;
}

template<typename ...Args>
std::string GenerateEditCommand(std::string const &table,
  std::string const &keyName,
  std::string const &key,
  Args... args)
{
  return "UPDATE " + table + " SET " + GenerateEditCommandInternal(args...) +
    " WHERE " + keyName + " = '" + key + "'";
}

inline QString GenerateEditCommand(std::string const &table,
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

#endif
