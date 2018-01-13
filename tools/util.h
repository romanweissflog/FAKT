#ifndef UTIL_H
#define UTIL_H

#include "dbf.h"

#include <string>
#include <map>

typedef struct _DBF_FIELD_DATA
{
  char     name[DBF_DBASE4_FIELDNAMELENGTH + 1];
  enum dbf_data_type type;
  size_t   length;
  dbf_uint cecCount;
  char*    ptr;
  uint32_t namehash;
} DBF_FIELD_DATA;

namespace util
{
  void SanitizeSqlCommand(std::string &sql);
  void ReplaceInvalid(std::string &sql, size_t pos);
  std::map<std::string, std::string> GetRow(DBF_HANDLE handle, size_t fields);
}

#endif
