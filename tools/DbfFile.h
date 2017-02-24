#ifndef DBF_FILE_H
#define DBF_FILE_H

#include <fstream>
#include <vector>

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

#pragma pack(push)
#pragma pack(1)
struct DbfRecord_s
{
	char archName[11];
	char chFieldType;

	uint32_t uDisplacement;
	uint8_t uLength;
	uint8_t uDecimalPlaces;
	uint8_t fFlags;

	uint32_t uNextValue;
	uint8_t uStepValue;
	char uReserved[8];
};
#pragma pack(pop)

class DbfFile_c
{
	public:
		DbfFile_c(const char *szFileName);

		void DumpAll(const char *szDestFileName);
		void DumpFields(const char *szDestFileName, const char **fields, size_t numFields);

	public:
		std::ifstream clFile;

		DbfHeader_s stHeader;
		std::vector<DbfRecord_s> vecRecords;
    std::vector<std::string> columnNames;
    std::vector<std::vector<std::string>> data;

		size_t szRowSize;
		size_t szLargestFieldSize;
};


#endif