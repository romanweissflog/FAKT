#include "sqlite3.h"

#include "util.h"

#include <Windows.h>

#include <filesystem>
#include <string>
#include <iostream>
#include <vector>
#include <regex>
#include <map>

using namespace std;

namespace
{
  vector<string> neededEntries
  {
    "POSIT",
    "ARTNR",
    "HAUPTARTBEZ",
    "ARTBEZ",
    "ME",
    "MENGE",
    "EP",
    "LP",
    "SP",
    "GP",
    "MP",
    "BAUZEIT",
    "P_RABATT",
    "EKP",
    "MULTI",
    "STUSATZ"
  };

  vector<string> GetFiles(string folder)
  {
    using std::experimental::filesystem::recursive_directory_iterator;
    using std::experimental::filesystem::directory_iterator;
    vector<string> names;
    auto addFile = [&names](std::experimental::filesystem::v1::directory_entry const &f)
    {
      string path = f.path().string();
      if (path.find_last_of(".") != string::npos)
      {
        if (path.substr(path.find_last_of("."), path.size()) == ".DBF")
        {
          names.push_back(path);
        }
      }
    };

    for (auto &&dirEntry : directory_iterator(folder))
    {
      addFile(dirEntry);
    }

    for (auto &&dirEntry : recursive_directory_iterator(folder))
    {
      addFile(dirEntry);
    }
    return names;
  }

  void FilterFiles(std::vector<string> &files, bool asInvoice)
  {
    if (asInvoice)
    {
      for (auto it = begin(files); it != end(files); )
      {
        auto const file = it->substr(it->find_last_of("\\"), it->size() - it->find_last_of("\\"));
        if (file.substr(3, 2) == "BA")
        {
          it = files.erase(it);
        }
        else
        {
          ++it;
        }
      }
    }
    else
    {
      for (auto it = begin(files); it != end(files); )
      {
        auto const file = it->substr(it->find_last_of("\\"), it->size() - it->find_last_of("\\"));
        if (file.substr(3, 2) != "BA")
        {
          it = files.erase(it);
        }
        else
        {
          ++it;
        }
      }
    }
  }

  vector<string> ManipulateOutputHeader(vector<string> const &oldHeader)
  {
    vector<string> output;
    for (auto &&e : oldHeader)
    {
      if (e.compare("ARTBEZ1") == 0)
      {
        output.push_back("ARTBEZ");
      }
      else if (find(begin(neededEntries), end(neededEntries), e) != end(neededEntries))
      {
        output.push_back(e);
      }
      else
      {
        continue;
      }
    }
    for (auto &&e : neededEntries)
    {
      if (find(begin(output), end(output), e) == end(output))
      {
        output.push_back(e);
      }
    }
    return output;
  }

  map<string, string> ManipulateOutputEntry(map<string, string> const &input)
  {
    map<string, string> output;
    for (auto &&e : input)
    {
      if (e.first.compare("ARTBEZ1") == 0)
      {
        output["HAUPTARTBEZ"] = e.second;
      }
      else if (e.first.compare("ARTBEZ2") == 0)
      {
        output["ARTBEZ"] = e.second;
      }
      else if (e.first.find("ARTBEZ") != string::npos)
      {
        output["ARTBEZ"] += "\n" + e.second;
      }
      else if (find(begin(neededEntries), end(neededEntries), e.first) != end(neededEntries))
      {
        output[e.first] = e.second;
      }
      else
      {
        continue;
      }
    }
    for (auto &&e : neededEntries)
    {
      if (output.count(e) == 0)
      {
        output[e] = " ";
      }
    }
    return output;
  }
}

int main(int argc, const char **argv)
{
  if (argc < 4)
  {
    cout << "Bad number of arguments\n";
    return -1;
  }
  try
  {
    string dst(argv[1]);

    // path to folder RP (AP) (containing rp / rpba (ap))
    string src(argv[2]);
    std::string type(argv[3]);  // R for invoice, A for offer, BA for jobsite
    auto files = GetFiles(src);
    if (type != "A")
    {
      FilterFiles(files, (type != "BA"));
    }

    sqlite3 *db = nullptr;
    int rc;
    sqlite3_stmt *stmt = nullptr;
    string sql;
    const char *tail;

    vector<string> result;

    rc = sqlite3_open(dst.c_str(), &db);
    if (rc != SQLITE_OK)
    {
      cout << "error code OPEN " << rc << endl;
      return -1;
    }

    size_t counter = 0;
    std::cout << files.size() << " files to be processed" << std::endl;

    std::regex rgx("(\\d+)");
    for (auto &&f : files)
    {
      if (f.find(".TXT") != string::npos)
      {
        continue;
      }

      cout << "Process: " << (float)counter / files.size() << endl;

      DBF_HANDLE handle = dbf_open(f.c_str(), NULL);
      if (handle == NULL)
      {
        throw std::runtime_error("Invalid dbf file");
      }
      size_t count = dbf_getrecordcount(handle);
      size_t fields = dbf_getfieldcount(handle);

      vector<string> columns;
      for (size_t i{}; i < fields; ++i)
      {
        auto const fieldPtr = (DBF_FIELD_DATA*)dbf_getfieldptr(handle, i);
        columns.push_back(string(fieldPtr->name));
      }

      size_t totalSize = columns.size();

      std::string tableNameTmp = f.substr(f.find_last_of("\\") + 2, f.find_last_of(".") - f.find_last_of("\\") - 2);
      auto regexRes = std::sregex_iterator(std::begin(tableNameTmp), std::end(tableNameTmp), rgx);
      std::string tableName = type + regexRes->begin()->str();
      if (type == "BA")
      {
        tableName.insert(2, "00");
      }

      sql = "DROP TABLE IF EXISTS " + tableName + ";";
      rc = sqlite3_prepare_v2(db, sql.c_str(), (int)sql.size(), &stmt, &tail);
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

      auto header = ManipulateOutputHeader(columns);

      sql = "CREATE TABLE IF NOT EXISTS " + tableName
        + "(id INTEGER PRIMARY KEY, ";
      for (auto &&h : header)
      {
        //if (h == "POSIT")
        //{
        //  sql += h + " TEXT UNIQUE, ";
        //}
        //else
        //{
          sql += h + " TEXT, ";
        //}
      }
      sql = sql.substr(0, sql.size() - 2);
      sql += ");";
      rc = sqlite3_prepare_v2(db, sql.c_str(), (int)sql.size(), &stmt, &tail);
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

      size_t failedEntries = 0;
      for (size_t i{}; i < count; i++)
      {
        dbf_setposition(handle, i);
        auto row = util::GetRow(handle, totalSize);
        auto entry = ManipulateOutputEntry(row);
        bool onlyEmpty = true;
        sql = "INSERT INTO " + tableName + " (";
        for (auto &&h : header)
        {
          sql += h + ", ";
        }
        sql = sql.substr(0, sql.size() - 2);
        sql += ") VALUES (";
        for (auto &&h : header)
        {
          if (entry[h].find_first_not_of(' ') != string::npos && entry[h].find_first_not_of('\0') != string::npos)
          {
            onlyEmpty = false;
          }
          sql += "'" + entry[h] + "', ";
        }
        sql = sql.substr(0, sql.size() - 2);
        sql += ");";

        util::SanitizeSqlCommand(sql);

        if (onlyEmpty)
        {
          continue;
        }

        rc = sqlite3_prepare_v2(db, sql.c_str(), (int)sql.size(), &stmt, &tail);
        if (rc != SQLITE_OK)
        {
          failedEntries++;
          //std::cout << sql << std::endl;
          //cout << "error code PREPARE ENTRY " << rc << endl;
          continue;
        }

        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE)
        {
          cout << "error code STEP ENTRY " << rc << endl;
          continue;
        }
      }
      string currentResult = tableName + ": FAILED: " + to_string(failedEntries) + " from " + to_string(count) + " entry.";
      result.push_back(currentResult);
      counter++;
      dbf_close(&handle);
    }
    for (auto &&s : result)
    {
      cout << s << endl;
    }

    rc = sqlite3_finalize(stmt);
    if (rc != SQLITE_OK)
    {
      cout << "eror code FINALIZE " << rc << "\n";
      return -1;
    }

    rc = sqlite3_close_v2(db);
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