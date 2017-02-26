#include "DbfFile.h"
#include "sqlite3.h"

#include <exception>
#include <iostream>
#include <vector>
#include <string>
#include <cstdint>
#include <map>

using namespace std;

namespace 
{
  namespace leistung
  {
    void manipulateOutputHeaderLeistung(vector<string> &input)
    {
      vector<string> output;
      for (auto &&e : input)
      {
        if (e.compare("ARTBEZ1") == 0)
        {
          output.push_back("ARTBEZ");
        }
        else if (e.find("ARTBEZ") != string::npos || e.find("RP") != string::npos ||
          e.compare("LT") == 0)
        {
          continue;
        }
        else
        {
          output.push_back(e);
        }
      }
      input = output;
    }

    void manipulateOutputEntryLeistung(map<string, string> &input)
    {
      map<string, string> output;
      for (auto &&e : input)
      {
        if (e.first.compare("ARTBEZ1") == 0)
        {
          output["ARTBEZ"] = e.second;
        }
        else if (e.first.compare("ARTBEZ20") == 0 || e.first.find("ARTBEZ1") != string::npos)
        {
          continue;
        }
        else if (e.first.find("ARTBEZ") != string::npos)
        {
          if (e.second[e.second.size() - 1] != '\0' && e.second[e.second.size() - 1] != ' ')
          {
            output.at("ARTBEZ") += "\n" + e.second;
          }
        }
        else if (e.first.compare("ME") == 0)
        {
          output[e.first] = e.second.substr(1, e.second.size());
        }
        else if(e.first.compare("RP") > 0 || e.first.compare("LT") == 0)
        {
          continue;
        }
        else
        {
          output[e.first] = e.second;
        }
      }
      input = output;
    }

    void manipulateInputLeistung(DbfRecord_s &record)
    {
      if (string(record.archName).compare("ARTBEZ20") == 0)
      {
        record.uLength += 6;
      }
      if (string(record.archName).compare("TRP") == 0)
      {
        record.uLength -= 6;
      }
    }
  }

  map<string, function<void(DbfRecord_s&)>> manipulateInput
  {
    { string("LEISTUNG"), leistung::manipulateInputLeistung }
  };

  map<string, function<void(vector<string>&)>> manipulateOutputHeader
  {
    { string("LEISTUNG"), leistung::manipulateOutputHeaderLeistung }
  };

  map<string, function<void(map<string, string>&)>> manipulateOuputEntry
  {
    {string("LEISTUNG"), leistung::manipulateOutputEntryLeistung}
  };
}

void SanitizeString(string &str)
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
      default: /*cout << str << " " << (int16_t)str.data()[i] << " ";*/ break;
      }
      i++;
    }
  }
}

int main(int argc, const char **argv)
{
  try
  {
    string folder = string(argv[1]);
    size_t nrTables = argc - 2;

    sqlite3 *db = nullptr;
    int rc;

    sqlite3_stmt *stmt = nullptr;
    string sql;
    const char *tail;
    vector<string> result;

    for (size_t i = 0; i < nrTables; i++)
    {
      string tableName(argv[i + 2]);
      string fileName = folder + tableName + ".DBF";
      DbfFile_c file(fileName.c_str(), manipulateInput[tableName]);
      file.DumpAll("output.txt");

      rc = sqlite3_open("fakt.db", &db);
      if (rc != SQLITE_OK)
      {
        cout << "error code OPEN " << rc << endl;
        return -1;
      }

      sql = "DROP TABLE IF EXISTS " + tableName + ";";
      rc = sqlite3_prepare(db, sql.c_str(), sql.size(), &stmt, &tail);
      if (rc != SQLITE_OK)
      {
        cout << "error code PREPARE DELETE " << rc << endl;
        return -1;
      }

      rc = sqlite3_step(stmt);
      if (rc != SQLITE_DONE)
      {
        cout << "error code DELETE " << rc << endl;
        return -1;
      }

      manipulateOutputHeader[tableName](file.columnNames);

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
        cout << "error code PREPARE HEADER " << rc << endl;
        return -1;
      }

      rc = sqlite3_step(stmt);
      if (rc != SQLITE_DONE)
      {
        cout << "error code STEP " << rc << endl;
        return -1;
      }

      size_t counter = 0;
      size_t failedEntries = 0;
     
      for (auto &&d : file.data)
      {
        manipulateOuputEntry[tableName](d);
        cout << "Process: " << (float)counter / file.data.size() << endl;
        sql = "INSERT INTO " + tableName + " (";
        for (auto &&h : file.columnNames)
        {
          sql += h + ", ";
        }
        sql = sql.substr(0, sql.size() - 2);
        sql += ") VALUES (";
        for (auto &&h : file.columnNames)
        {
          SanitizeString(d[h]);
          sql += "'" + d[h] + "', ";
        }
        sql = sql.substr(0, sql.size() - 2);
        sql += ");";

        rc = sqlite3_prepare(db, sql.c_str(), sql.size(), &stmt, &tail);
        if (rc != SQLITE_OK)
        {
          failedEntries++;
          cout << "error code PREPARE ENTRY " << rc << endl;
          continue;
        }

        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE)
        {
          cout << "error code STEP ENTRY " << rc << endl;
          return -1;
        }
        counter++;
      }
      string currentResult = tableName + ": FAILED: " + to_string(failedEntries) + " from " + to_string(file.data.size()) + " entry.";
      result.push_back(currentResult);
      cout << currentResult << endl;
    }
    for (auto &&s : result)
    {
      cout << s << endl;
    }

    rc = sqlite3_close(db); 
    if (rc != SQLITE_OK)
    {
      cout << "error code CLOSE " << rc << endl;
      return -1;
    }
  }
  catch (const exception &e)
  {
    cerr << e.what();
    return -1;
  }
  return 0;
}