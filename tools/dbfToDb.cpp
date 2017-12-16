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
    void manipulateOutputHeader(vector<string> &input)
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

    void manipulateOutputEntry(map<string, string> &input)
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
  }

  namespace material
  {
    void manipulateOutputHeader(vector<string> &input)
    {
      vector<string> output;
      for (auto &&e : input)
      {
        if (e.compare("ARTBEZ1") == 0)
        {
          output.push_back("ARTBEZ");
        }
        else if (e.compare("EKP1") == 0)
        {
          output.push_back("EKP");
        }
        else if (e.compare("EP1") == 0)
        {
          output.push_back("NETTO");
        }
        else if (e.compare("EP1B") == 0)
        {
          output.push_back("BRUTTO");
        }
        else if (e.compare("BAUZEIT") == 0
          || e.compare("ME") == 0
          || e.compare("ARTNR") == 0)
        {
          output.push_back(e);
        }
        else
        {
          continue;
        }
      }
      output.push_back("VERARB");
      input = output;
    }

    void manipulateOutputEntry(map<string, string> &input)
    {
      map<string, string> output;
      for (auto &&e : input)
      {
        if (e.first.compare("ARTNR") == 0
          || e.first.compare("ME") == 0
          || e.first.compare("BESTAND") == 0
          || e.first.compare("BAUZEIT") == 0
          || e.first.compare("LIEFERER") == 0)
        {
          output[e.first] = e.second;
        }
        else if (e.first.compare("ARTBEZ1") == 0)
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
        else if (e.first.compare("EKP1") == 0)
        {
          output["EKP"] = e.second;
        }
        else if (e.first.compare("EP1") == 0)
        {
          output["NETTO"] = e.second;
        }
        else if (e.first.compare("EP1B") == 0)
        {
          output["BRUTTO"] = e.second;
        }
      }
      output["VERARB"] = "";
      input = output;
    }
  }

  namespace adressen
  {
    void manipulateOutputHeader(vector<string> &input)
    {
      std::vector<string> output;
      for (auto &&e : input)
      {
        if (e.compare("SUCHNAME") == 0
          || e.compare("KUNR") == 0
          || e.compare("ANREDE") == 0
          || e.compare("STRASSE") == 0
          || e.compare("PLZ") == 0
          || e.compare("ORT") == 0
          || e.compare("FAX") == 0
          || e.compare("TELEFON") == 0)
        {
          output.push_back(e);
        }
        else if (e.compare("NAME1") == 0)
        {
          output.push_back("NAME");
        }
        else
        {
          continue;
        }
      }
      input = output;
    }

    void manipulateOutputEntry(map<string, string> &input)
    {
      map<string, string> output;
      for (auto &&e : input)
      {
        if (e.first.compare("SUCHNAME") == 0
          || e.first.compare("KUNR") == 0
          || e.first.compare("ANREDE") == 0
          || e.first.compare("STRASSE") == 0
          || e.first.compare("PLZ") == 0
          || e.first.compare("ORT") == 0
          || e.first.compare("FAX") == 0
          || e.first.compare("TELEFON") == 0)
        {
          output[e.first] = e.second;
        }
        else if (e.first.compare("NAME1") == 0)
        {
          output["NAME"] = e.second;
        }
        else if (e.first.compare("NAME2") == 0)
        {
          if (e.second.at(0) != ' ')
          {
            output["NAME"] += " " + e.second;
          }
        }
        else
        {
          continue;
        }
      }
      input = output;
    }
  }

  namespace rechnung
  {
    void manipulateOutputHeader(vector<string> &input)
    {
      vector<string> output;
      for (auto &&e : input)
      {
        if (e.compare("NAME2") == 0
          || e.compare("GRGESAMT") == 0
          || e.compare("TRGESAMT") == 0
          || (e.find("HEADLIN") != string::npos && e.compare("HEADLIN1") != 0)
          || e.compare("BESTDAT") == 0
          || e.compare("RABATT") == 0
          || (e.find("SCHLUSS") != string::npos && e.compare("SCHLUSS1") != 0)
          || e.find("MAHNDAT") != string::npos
          || e.find("GEMAHNT") != string::npos
          || e.find("FELD") != string::npos
          || e.find("STAT_GR") == 0
          || e.compare("UNS_ZEICH") == 0
          || e.find("INZAH") != string::npos
          || e.compare("ANZAHLUNG") == 0
          || e.compare("KALK_ART") == 0
          || e.compare("GEDRUCKT") == 0
          || e.compare("KST") == 0
          || e.find("GEMAHNDAT") != string::npos
          || e.compare("SICHERHEIT") == 0
          || e.compare("RAB_TEXT") == 0
          || e.compare("KONTOSOLL") == 0
          || e.compare("RAB_EXT") == 0
          || e.compare("STATUS") == 0
          || e.compare("BAUSTNR") == 0
          || e.compare("LAST") == 0
          || e.compare("WEU") == 0)
        {
          continue;
        }
        else if (e.compare("NAME1") == 0)
        {
          output.push_back("NAME");
        }
        else if (e.compare("HEADLIN1") == 0)
        {
          output.push_back("HEADLIN");
        }
        else if (e.compare("SCHLUSS1") == 0)
        {
          output.push_back("SCHLUSS");
        }
        else
        {
          output.push_back(e);
        }
      }
      input = output;
    }

    void manipulateOutputEntry(map<string, string> &input)
    {
      map<string, string> output;
      for (auto &&e : input)
      {
        if (e.first.compare("GRGESAMT") == 0
          || e.first.compare("TRGESAMT") == 0
          || e.first.compare("BESTDAT") == 0
          || e.first.compare("RABATT") == 0
          || e.first.find("MAHNDAT") != string::npos
          || e.first.find("GEMAHNT") != string::npos
          || e.first.find("FELD") != string::npos
          || e.first.compare("STAT_GR") == 0
          || e.first.compare("UNS_ZEICH") == 0
          || e.first.find("INZAH") != string::npos
          || e.first.compare("ANZAHLUNG") == 0
          || e.first.compare("KALK_ART") == 0
          || e.first.compare("GEDRUCKT") == 0
          || e.first.compare("KST") == 0
          || e.first.find("GEMAHNDAT") != string::npos
          || e.first.compare("SICHERHEIT") == 0
          || e.first.compare("RAB_TEXT") == 0
          || e.first.compare("RAB_EXT") == 0
          || e.first.compare("STATUS") == 0
          || e.first.compare("BAUSTNR") == 0
          || e.first.compare("LAST") == 0
          || e.first.compare("WEU") == 0)
        {
          continue;
        }
        else if (e.first.compare("NAME1") == 0)
        {
          output["NAME"] = e.second;
        }
        else if (e.first.find("NAME") != string::npos)
        {
          output["NAME"] += " " + e.second;
        }
        else if (e.first.compare("HEADLIN1") == 0)
        {
          output["HEADLIN"] = e.second;
        }
        else if (e.first.find("HEADLIN") != string::npos)
        {
          if (e.second.at(0) != ' ')
          {
            output["HEADLIN"] += "\n" + e.second;
          }
        }
        else if (e.first.compare("SCHLUSS1") == 0)
        {
          output["SCHLUSS"] = e.second;
        }
        else if (e.first.find("SCHLUSS") != string::npos)
        {
          if (e.second.at(0) != ' ')
          {
            output["SCHLUSS"] + "\n" + e.second;
          }
        }
        else if (e.first.compare("REDAT") == 0)
        {
          if (e.second.size() == 8)
          {
            output[e.first] = e.second.substr(0, 4) + "-" + e.second.substr(4, 2) + "-" + e.second.substr(6, 2);
          }
          else
          {
            output[e.first] = "";
          }
        }
        else
        {
          output[e.first] = e.second;
        }
      }
      input = output;
    }
  }

  namespace angebot
  {
    void manipulateOutputHeader(vector<string> &input)
    {
      vector<string> output;
      for (auto &&e : input)
      {
        if (e.compare("AKTUELL") == 0
          || e.compare("GRGESAMT") == 0
          || e.compare("TRGESAMT") == 0
          || (e.find("HEADLIN") != string::npos && e.compare("HEADLIN1") != 0)
          || e.compare("RABATT") == 0
          || e.compare("BESTDAT") == 0
          || (e.find("SCHLUSS") != string::npos && e.compare("SCHLUSS1") != 0)
          || e.compare("NAME2") == 0
          || e.compare("ANG_TEXT") == 0
          || e.compare("KALK_ART") == 0
          || e.compare("AKTUELL") == 0
          || e.compare("KONTOSOLL") == 0
          || e.compare("KST") == 0
          || e.find("GEMAHNDAT") != string::npos
          || e.compare("SICHERHEIT") == 0
          || e.compare("STATUS") == 0
          || e.compare("BAUSTNR") == 0
          || e.compare("LAST") == 0
          || e.compare("WEU") == 0
          || e.compare("RAB_TEXT") == 0
          || e.compare("UNS_ZEICH") == 0
          || e.compare("LIEFBED") == 0)
        {
          continue;
        }
        else if (e.compare("NAME1") == 0)
        {
          output.push_back("NAME");
        }
        else if (e.compare("HEADLIN1") == 0)
        {
          output.push_back("HEADLIN");
        }
        else if (e.compare("SCHLUSS1") == 0)
        {
          output.push_back("SCHLUSS");
        }
        else
        {
          output.push_back(e);
        }
      }
      input = output;
    }

    void manipulateOutputEntry(map<string, string> &input)
    {
      map<string, string> output;
      for (auto &&e : input)
      {
        if ( e.first.compare("AKTUELL") == 0
          || e.first.compare("GRGESAMT") == 0
          || e.first.compare("TRGESAMT") == 0
          || e.first.compare("RABATT") == 0
          || e.first.compare("BESTDAT") == 0
          || e.first.find("MAHNDAT") != string::npos
          || e.first.find("GEMAHNT") != string::npos
          || e.first.find("FELD") != string::npos
          || e.first.find("INZAH") != string::npos
          || e.first.compare("ANZAHLUNG") == 0
          || e.first.compare("ANG_TEXT") == 0
          || e.first.compare("KALK_ART") == 0
          || e.first.compare("AKTUELL") == 0
          || e.first.compare("KONTOSOLL") == 0
          || e.first.compare("KST") == 0
          || e.first.find("GEMAHNDAT") != string::npos
          || e.first.compare("SICHERHEIT") == 0
          || e.first.compare("STATUS") == 0
          || e.first.compare("BAUSTNR") == 0
          || e.first.compare("LAST") == 0
          || e.first.compare("WEU") == 0
          || e.first.compare("RAB_TEXT") == 0
          || e.first.compare("UNS_ZEICH") == 0
          || e.first.compare("LIEFBED") == 0)
        {
          continue;
        }
        else if (e.first.compare("NAME1") == 0)
        {
          output["NAME"] = e.second;
        }
        else if (e.first.find("NAME") != string::npos)
        {
          output["NAME"] += " " + e.second;
        }
        else if (e.first.compare("HEADLIN1") == 0)
        {
          output["HEADLIN"] = e.second;
        }
        else if (e.first.find("HEADLIN") != string::npos)
        {
          if (e.second.at(0) != ' ')
          {
            output["HEADLIN"] += "\n" + e.second;
          }
        }
        else if (e.first.compare("SCHLUSS1") == 0)
        {
          output["SCHLUSS"] = e.second;
        }
        else if (e.first.find("SCHLUSS") != string::npos)
        {
          if (e.second.at(0) != ' ')
          {
            output["SCHLUSS"] + "\n" + e.second;
          }
        }
        else if (e.first.compare("REDAT") == 0)
        {
          if (e.second.size() == 8)
          {
            output[e.first] = e.second.substr(0, 4) + "-" + e.second.substr(4, 2) + "-" + e.second.substr(6, 2);
          }
          else
          {
            output[e.first] = "";
          }
        }
        else
        {
          output[e.first] = e.second;
        }
      }
      input = output;
    }
  }

  map<string, string> manipulateTableName
  {
    { "LEISTUNG", "LEISTUNG" },
    { "MATERIAL", "MATERIAL" },
    { "ADRESSEN", "ADRESSEN" },
    { "RECHNUNG", "RECHNUNG" },
    { "ANGEBOT",  "ANGEBOT"  }
  };

  map<string, function<void(vector<string>&)>> manipulateOutputHeader
  {
    { "LEISTUNG", leistung::manipulateOutputHeader },
    { "MATERIAL", material::manipulateOutputHeader },
    { "ADRESSEN", adressen::manipulateOutputHeader },
    { "RECHNUNG", rechnung::manipulateOutputHeader },
    { "ANGEBOT",  angebot::manipulateOutputHeader  }
  };

  map<string, function<void(map<string, string>&)>> manipulateOutputEntry
  {
    { "LEISTUNG", leistung::manipulateOutputEntry },
    { "MATERIAL", material::manipulateOutputEntry },
    { "ADRESSEN", adressen::manipulateOutputEntry },
    { "RECHNUNG", rechnung::manipulateOutputEntry },
    { "ANGEBOT",  angebot::manipulateOutputEntry  }
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
  if (argc < 3)
  {
    std::cout << "Missing arguments\n";
    return -1;
  }
  try
  {
    string dst = string(argv[1]);
    string folder = string(argv[2]);
    size_t nrTables = argc - 3;

    sqlite3 *db = nullptr;
    int rc;

    sqlite3_stmt *stmt = nullptr;
    string sql;
    const char *tail;
    vector<string> result;

    for (size_t i = 0; i < nrTables; i++)
    {
      string tableName(argv[i + 3]);
      string fileName = folder + tableName + ".DBF";
      DbfFile_c file(fileName.c_str());
      file.DumpAll("output.txt");

      rc = sqlite3_open(dst.c_str(), &db);
      if (rc != SQLITE_OK)
      {
        cout << "error code OPEN " << rc << endl;
        return -1;
      }

      tableName = manipulateTableName[tableName];
      if (tableName == "RECHNUNG")
      {
        sql = "DROP TABLE IF EXISTS BAUSTELLE;";
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

      if (tableName == "RECHNUNG")
      {
        sql = "CREATE TABLE IF NOT EXISTS BAUSTELLE (id INTEGER PRIMARY KEY, ";
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
     
      string currentTable = tableName;
      for (auto &&d : file.data)
      {
        manipulateOutputEntry[tableName](d);

        if (tableName == "RECHNUNG")
        {
          if (d["RENR"].substr(0, 2) == "BA")
          {
            currentTable = "BAUSTELLE";
            d["RENR"] = d["RENR"].substr(2, d["RENR"].size());
          }
          else
          {
            currentTable = "RECHNUNG";
          }
        }

        cout << "Process: " << (float)counter / file.data.size() << endl;
        sql = "INSERT INTO " + currentTable + " (";
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

    rc = sqlite3_finalize(stmt);
    if (rc != SQLITE_OK)
    {
      cout << "eror code FINALIZE " << rc << "\n";
      return -1;
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