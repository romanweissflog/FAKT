#include "sqlite3.h"
#include "DbfFile.h"

#include <Windows.h>

#include <experimental\filesystem>
#include <string>
#include <iostream>
#include <vector>
#include <regex>

using namespace std;

namespace
{
  vector<string> neededEntries
  {
    "POSIT",
    "ARTBEZ",
    "ME",
    "MENGE",
    "EP",
    "LP",
    "SP",
    "GP",
    "BAUZEIT",
    "P_RABATT",
    "EKP",
    "LPKORR",
    "MULTI",
    "STUSATZ"
  };

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
        case -3: str.replace(i, 1, 1, '\0'); break;
        default: /*cout << str << " " << (int16_t)str.data()[i] << " ";*/ break;
        }
        i++;
      }
      if (str.data()[str.size() - 1] == '\0')
      {
        str = str.substr(0, str.size() - 1);
      }
    }
  }

  vector<string> GetFilesOld(string folder)
  {
    vector<string> names;
    string search_path = folder + "/*.*";
    WIN32_FIND_DATA fd;
    HANDLE hFind = ::FindFirstFile(search_path.c_str(), &fd);
    if (hFind != INVALID_HANDLE_VALUE) {
      do {
        // read all (real) files in current folder
        // , delete '!' read other 2 default folder . and ..
        if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
          names.push_back(folder + "\\" + fd.cFileName);
        }
      } while (::FindNextFile(hFind, &fd));
      ::FindClose(hFind);
    }
    return names;
  }

  vector<string> GetFiles(string folder)
  {
    using std::experimental::filesystem::recursive_directory_iterator;
    vector<string> names;
    for (auto &&dirEntry : recursive_directory_iterator(folder))
    {
      string path = dirEntry.path().string();
      if (path.find_last_of(".") != string::npos)
      {
        if (path.substr(path.find_last_of("."), path.size()) == ".DBF")
        {
          names.push_back(path);
        }
      }
    }
    return names;
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
        output["ARTBEZ"] = e.second;
      }
      else if (e.first.find("ARTBEZ") != string::npos)
      {
        if (e.second[e.second.size() - 1] != -3)
        {
          output["ARTBEZ"] += "\n" + e.second;
        }
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
  try
  {
    string folder = string("C:\\Users\\Roman\\Desktop\\vati\\fakt\\RP");
    auto files = GetFiles(folder);

    sqlite3 *db = nullptr;
    int rc;
    sqlite3_stmt *stmt = nullptr;
    string sql;
    const char *tail;

    vector<string> result;

    rc = sqlite3_open("invoices.db", &db);
    if (rc != SQLITE_OK)
    {
      cout << "error code OPEN " << rc << endl;
      return -1;
    }

    size_t counter = 0;
    std::cout << files.size() << " files to be processed" << std::endl;

    std::regex lastInvoiceRegex("(\\d+)");
    for (auto &&f : files)
    {
      if (f.find(".TXT") != string::npos)
      {
        continue;
      }

      cout << "Process: " << (float)counter / files.size() << endl;

      DbfFile_c file(f.c_str());
      file.DumpAll("output.txt");

      std::string tableNameTmp = f.substr(f.find_last_of("\\") + 2, f.find_last_of(".") - f.find_last_of("\\") - 2);
      auto regexRes = std::sregex_iterator(std::begin(tableNameTmp), std::end(tableNameTmp), lastInvoiceRegex);
      std::string tableName = std::string("R") + regexRes->begin()->str();

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

      auto header = ManipulateOutputHeader(file.columnNames);

      sql = "CREATE TABLE IF NOT EXISTS " + tableName
        + "(id INTEGER PRIMARY KEY, ";
      for (auto &&h : header)
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

      size_t failedEntries = 0;
      for (auto &&d : file.data)
      {
        auto entry = ManipulateOutputEntry(d);
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
          SanitizeString(entry[h]);
          if (entry[h].find_first_not_of(' ') != string::npos && entry[h].find_first_not_of('\0') != string::npos)
          {
            onlyEmpty = false;
          }
          sql += "'" + entry[h] + "', ";
        }
        sql = sql.substr(0, sql.size() - 2);
        sql += ");";

        if (onlyEmpty)
        {
          continue;
        }

        rc = sqlite3_prepare(db, sql.c_str(), sql.size(), &stmt, &tail);
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
      string currentResult = tableName + ": FAILED: " + to_string(failedEntries) + " from " + to_string(file.data.size()) + " entry.";
      result.push_back(currentResult);
      counter++;
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