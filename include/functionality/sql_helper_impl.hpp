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

inline std::string GenerateEditCommandInternal(SqlPair const &current)
{
  return current.key + " = " + current.value;
}

template<typename ...Args>
inline std::string GenerateEditCommandInternal(SqlPair const &current, Args... args)
{
  return current.key + " = " + current.value + ", " + GenerateEditCommandInternal(args...);
}

template<typename ...Args>
std::string GenerateInsertCommand(std::string const &table, Args... args)
{
  std::string skeleton = "() VALUES ()";
  return "INSERT INTO " + table + " " + GenerateInsertCommandInternal(skeleton, args...);
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

#endif
