#include "DbfFile.h"
#include "sqlite3.h"

#include <exception>
#include <iostream>
#include <vector>
#include <string>
#include <cstdint>

void SanitizeString(std::string &str)
{
  for (size_t i = 0; ; i++)
  {
    if (i >= str.size())
    {
      break;
    }
    if (str.data()[i] < 0)
    {
      switch ((int16_t)str.data()[i])
      {
      case -31: str.insert(i, 1, 0xc3); str.replace(i + 1, 1, 1, 0x9f); break;  //ß
      case -97: str.insert(i, 1, 0xc3); str.replace(i + 1, 1, 1, 0xa4); break;  //ä
      case -103:
      case -106: str.insert(i, 1, 0xc3); str.replace(i + 1, 1, 1, 0x96); break; //Ö
      case -108: str.insert(i, 1, 0xc3); str.replace(i + 1, 1, 1, 0xb6); break; //ö
      case -127:
      case -68: str.insert(i, 1, 0xc3); str.replace(i + 1, 1, 1, 0xbc); break; //ü
      default: /*std::cout << str << " " << (int16_t)str.data()[i] << " ";*/ break;
      }
      i++;
    }
  }
}

int main(int argc, const char **argv)
{
  try
  {
    std::string folder = std::string(argv[1]);
    size_t nrTables = argc - 2;

    sqlite3 *db = nullptr;
    int rc;

    sqlite3_stmt *stmt = nullptr;
    std::string sql;
    const char *tail;
    std::vector<std::string> result;

    for (size_t i = 0; i < nrTables; i++)
    {
      std::string fileName = folder + std::string(argv[i + 2]) + ".DBF";
      DbfFile_c file(fileName.c_str());
      file.DumpAll("output.txt");

      std::string tableName = std::string(argv[i + 2]);
      rc = sqlite3_open("fakt.db", &db);
      if (rc != SQLITE_OK)
      {
        std::cout << "error code OPEN " << rc << std::endl;
        return -1;
      }

      sql = "DROP TABLE IF EXISTS " + tableName + ";";
      rc = sqlite3_prepare(db, sql.c_str(), sql.size(), &stmt, &tail);
      if (rc != SQLITE_OK)
      {
        std::cout << "error code PREPARE DELETE " << rc << std::endl;
        return -1;
      }

      rc = sqlite3_step(stmt);
      if (rc != SQLITE_DONE)
      {
        std::cout << "error code DELETE " << rc << std::endl;
        return -1;
      }

      sql = "CREATE TABLE IF NOT EXISTS " + tableName
        + "(id INTEGER PRIMARY KEY, ";
      for (auto &&h : file.columnNames)
      {
        sql += h + " TEXT, ";
      }
      sql = sql.substr(0, sql.size() - 2);
      sql += ");";
      rc = sqlite3_prepare(db, sql.c_str(), sql.size(), &stmt, &tail);
      if (rc != SQLITE_OK)
      {
        std::cout << "error code PREPARE HEADER " << rc << std::endl;
        return -1;
      }

      rc = sqlite3_step(stmt);
      if (rc != SQLITE_DONE)
      {
        std::cout << "error code STEP " << rc << std::endl;
        return -1;
      }

      size_t counter = 0;
      size_t failedEntries = 0;
      for (auto &&d : file.data)
      {
        std::cout << "Process: " << (float)counter / file.data.size() << std::endl;
        sql = "INSERT INTO " + tableName + " (";
        for (auto &&h : file.columnNames)
        {
          sql += h + ", ";
        }
        sql = sql.substr(0, sql.size() - 2);
        sql += ") VALUES (";
        for (auto &&e : d)
        {
          SanitizeString(e);
          sql += "'" + e + "', ";
        }
        sql = sql.substr(0, sql.size() - 2);
        sql += ");";

        rc = sqlite3_prepare(db, sql.c_str(), sql.size(), &stmt, &tail);
        if (rc != SQLITE_OK)
        {
          failedEntries++;
          std::cout << "error code PREPARE ENTRY " << rc << std::endl;
          continue;
        }

        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE)
        {
          std::cout << "error code STEP ENTRY " << rc << std::endl;
          return -1;
        }
        counter++;
      }
      std::string currentResult = tableName + ": FAILED: " + std::to_string(failedEntries) + " from " + std::to_string(file.data.size()) + " entry.";
      result.push_back(currentResult);
      std::cout << currentResult << std::endl;
    }
    for (auto &&s : result)
    {
      std::cout << s << std::endl;
    }

    rc = sqlite3_close(db); 
    if (rc != SQLITE_OK)
    {
      std::cout << "error code CLOSE " << rc << std::endl;
      return -1;
    }
  }
  catch (const std::exception &e)
  {
    std::cerr << e.what();
    return -1;
  }
  return 0;
}