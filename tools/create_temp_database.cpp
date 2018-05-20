#include "sqlite3.h"

#include <iostream>
#include <string>

using namespace std;

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    cout << "Missing arguments\n";
    return -1;
  }
  try
  {
    string dst(argv[1]);

    sqlite3 *db = nullptr;
    int rc;

    sqlite3_stmt *stmt = nullptr;
    string sql;
    const char *tail;

    rc = sqlite3_open(dst.c_str(), &db);
    if (rc != SQLITE_OK)
    {
      cout << "error code OPEN " << rc << endl;
      return -1;
    }

    sql = "CREATE TABLE IF NOT EXISTS GROUPS (POS TEXT UNIQUE, ARTBEZ TEXT, BRUTTO TEXT);";
    rc = sqlite3_prepare_v2(db, sql.c_str(), static_cast<int32_t>(sql.size()), &stmt, &tail);
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

    sql = "CREATE TABLE IF NOT EXISTS EXTRA_INFO (TYP TEXT, SKONTO TEXT, RABATT TEXT);";
    rc = sqlite3_prepare_v2(db, sql.c_str(), static_cast<int32_t>(sql.size()), &stmt, &tail);
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
  catch (...)
  {
    std::cout << "Unknown exception\n";
  }
  return 0;
}