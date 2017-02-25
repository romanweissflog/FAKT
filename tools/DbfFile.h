#ifndef DBF_FILE_H
#define DBF_FILE_H

#include <fstream>
#include <vector>
#include <functional>
#include <map>

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

struct DbfHeader_s
{
	uint8_t iType;
	char arcLastUpdate[3];

	uint32_t uNumRecords;

	uint16_t uFirstRecordOffset;
	uint16_t uRecordSize;

	char uReserved[15];
	uint8_t fFlags;
	uint8_t uCodePageMark;

	char uReserved2[2];
};

struct DbfRecord_s
{
	char archName[11];
	char chFieldType;
  char uReserved1[4];
	uint8_t uLength;
  uint8_t uDecCount;
  char uReserved2[14];
};

class DbfFile_c
{
	public:
		DbfFile_c(const char *szFileName, std::function<void(DbfRecord_s&)> manipulate);

		void DumpAll(const char *szDestFileName);

	public:
		std::ifstream clFile;

		DbfHeader_s stHeader;
		std::vector<DbfRecord_s> vecRecords;
    std::vector<std::string> columnNames;
    std::vector<std::map<std::string, std::string>> data;

		size_t szRowSize;
		size_t szLargestFieldSize;
};


#endif