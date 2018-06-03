#include "util.h"

#include "utf_8.h"

namespace util
{
  void ReplaceInvalid(std::string &sql, size_t pos)
  {
    int val = (int)sql[pos];
    if ((int)sql[pos - 1] < 0)
    {
      pos--;
      val = (int)sql[pos];
    }
    std::string newChar1, newChar2;
    sql.erase(pos, 1);
    switch (val)
    {
    case -60: newChar1 = 0xc3; newChar2 = 0x84; break; // Ae
    case -28: newChar1 = 0xc3; newChar2 = 0xa4; break; // ae
    case -36: newChar1 = 0xc3; newChar2 = 0x9c;  break; // Ue
    case -4: newChar1 = 0xc3; newChar2 = 0xbc;  break; // ue
    case -42: newChar1 = 0xc3; newChar2 = 0x96; break; // Oe
    case -10: newChar1 = 0xc3; newChar2 = 0xb6; break; // oe
    case -33: newChar1 = 0xc3; newChar2 = 0x9f;  break; // ss
    case -80: newChar1 = 0xc2; newChar2 = 0xba; break; // degree
    case -89: newChar1 = 0xd2; newChar2 = 0xa7; break;  // paragraph
    case -94: default: newChar1 = 0x20; newChar2 = 0x20; break;  // unknown
    }
    sql.insert(pos, newChar1);
    sql.insert(pos + 1, newChar2);
  }

  void SanitizeSqlCommand(std::string &sql)
  {
    if (!utf8::is_valid(sql.begin(), sql.end()))
    {
      auto invalidIt = utf8::find_invalid(sql.begin(), sql.end());
      while (invalidIt != sql.end())
      {
        ReplaceInvalid(sql, distance(begin(sql), invalidIt));
        invalidIt = utf8::find_invalid(std::begin(sql), sql.end());
      }
    }
  }

  std::map<std::string, std::string> GetRow(DBF_HANDLE handle, size_t fields)
  {
    std::map<std::string, std::string> row;
    for (size_t j{}; j < fields; j++)
    {
      char temp[80] = "";

      dbf_getfield(handle, dbf_getfieldptr(handle, static_cast<dbf_uint>(j)), temp, sizeof(temp), DBF_DATA_TYPE_ANY);
      auto column = (DBF_FIELD_DATA*)(dbf_getfieldptr(handle, static_cast<dbf_uint>(j)));
      row.emplace(std::string(column->name), std::string(temp));
    }
    return row;
  }
}