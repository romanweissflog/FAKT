#ifndef SQL_HELPER_H_
#define SQL_HELPER_H_

#include "data_entries.h"

#include <string>
#include "QtCore\qstring.h"

QString GenerateInsertCommand(std::string const &table, 
  DatabaseDataEntry::const_iterator begin, DatabaseDataEntry::const_iterator end);

QString GenerateEditCommand(std::string const &table,
  QString const &keyName,
  QString const &key,
  DatabaseDataEntry::const_iterator begin, DatabaseDataEntry::const_iterator end);

QString GenerateSelectAllCommand(std::string const &table,
  DatabaseDataEntry::const_iterator begin, DatabaseDataEntry::const_iterator end);

QString GenerateSelectRowCommand(std::string const &table,
  QString const &key, QString const &entry, 
  DatabaseDataEntry::const_iterator begin, DatabaseDataEntry::const_iterator end);

#endif
