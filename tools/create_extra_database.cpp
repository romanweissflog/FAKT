#include "sqlite3.h"

#include "QtSql\qsqldatabase.h"
#include "QtSql\qsqlquery.h"
#include "qimage.h"
#include "qbuffer.h"
#include "qimagewriter.h"
#include "qvariant.h"
#include "QtSql\qsqlerror.h"

#include <iostream>
#include <string>

using namespace std;

int DoWithCSqlLite(std::string const &dst)
{
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
    cout << "error code PREPARE GROUPS " << rc << endl;
    return -1;
  }
  rc = sqlite3_step(stmt);
  if (rc != SQLITE_DONE)
  {
    cout << "error code STEP " << rc << endl;
    return -1;
  }

  sql = "CREATE TABLE IF NOT EXISTS PRINT_DATA (TYP TEXT, SKONTO TEXT, RABATT TEXT, HEADLIN TEXT);";
  rc = sqlite3_prepare_v2(db, sql.c_str(), static_cast<int32_t>(sql.size()), &stmt, &tail);
  if (rc != SQLITE_OK)
  {
    cout << "error code PREPARE PRINT_DATA HEADER " << rc << endl;
    return -1;
  }
  rc = sqlite3_step(stmt);
  if (rc != SQLITE_DONE)
  {
    cout << "error code STEP " << rc << endl;
    return -1;
  }

  sqlite3_finalize(stmt);
  sqlite3_close(db);
  return 0;
}

int DoWithQSqlite(std::string const &dst, std::string const &logo)
{
  QSqlDatabase db(QSqlDatabase::addDatabase("QSQLITE", "main"));
  db.setDatabaseName(QString::fromStdString(dst));
  auto rc = db.open();

  QSqlQuery query(db);
  rc = query.exec("DROP TABLE IF EXISTS IMAGE_DATA");
  if (!rc)
  {
    cout << "error code deleting IMAGE_DATA " << query.lastError().text().toStdString() << endl;
    return -1;
  }
  rc = query.exec("CREATE TABLE IF NOT EXISTS IMAGE_DATA (id INTEGER PRIMARY KEY, TYPE TEXT, IMAGE BLOB)");
  if (!rc)
  {
    cout << "error code PREPARE image HEADER " << query.lastError().text().toStdString() << endl;
    return -1;
  }
  rc = query.prepare("INSERT INTO IMAGE_DATA (TYPE, IMAGE) VALUES('logo', :IM)");
  if (!rc)
  {
    cout << "error code PREPARE image " << query.lastError().text().toStdString() << endl;
    return -1;
  }

  QImage img;
  img.load(QString::fromStdString(logo));
  img.scaled(static_cast<int>(0.5 * img.width()), static_cast<int>(0.5 * img.height()), Qt::AspectRatioMode::KeepAspectRatio);
  QBuffer buffer;
  QImageWriter writer(&buffer, "PNG");
  writer.write(img);
  QByteArray data = buffer.data();

  query.bindValue(QString(":IM"), data);
  rc = query.exec();
  if (!rc)
  {
    cout << "error code inserting image text " << query.lastError().text().toStdString() << endl;
    return -1;
  }
  db.close();
  return 0;
}

int main(int argc, char* argv[])
{
  if (argc < 3)
  {
    cout << "Usage: <create_extra_database.exe> <path to database> <path to logo image>";
    return -1;
  }
  try
  {
    string dst(argv[1]);
    string img(argv[2]);

    DoWithCSqlLite(dst);
    DoWithQSqlite(dst, img);
  }
  catch (...)
  {
    std::cout << "Unknown exception\n";
  }
  return 0;
}