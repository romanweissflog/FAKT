/*
db2txt
Copyright (c) 2011 Bruno Sanches  http://code.google.com/p/dbf2txt

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose, 
including commercial applications, and to alter it and redistribute it freely, 
subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/


#include "DbfFile.h"

#include <iostream>
#include <sstream>
#include <algorithm>
#include <string>

namespace
{
  int ToHeaderName(char *archName)
  {
    int firstZero = 0;
    for (int i = 0; i < 11; i++)
    {
      if (archName[i] == 0x00)
      {
        firstZero = i;
        break;
      }
    }
    return firstZero;
  }
}

DbfFile_c::DbfFile_c(const char *szFileName, std::function<void(DbfRecord_s&)> manipulate):
	clFile(szFileName, std::ios_base::binary | std::ios_base::in)
{
	if(!clFile.good())
		throw std::exception("Cannot open file");

	clFile.read(reinterpret_cast<char *>(&stHeader), sizeof(stHeader));
	size_t sz = sizeof(DbfRecord_s);

	szRowSize = 0;
	szLargestFieldSize = 0;
	for(;;)
	{
		char end;
		clFile.read(&end, 1);
		if(end == 0x0D)
			break;

		vecRecords.push_back(DbfRecord_s());
		DbfRecord_s &record = vecRecords.back();

		memcpy(&record, &end, 1);
		clFile.read(reinterpret_cast<char *>(&record)+1, sizeof(DbfRecord_s)-1);

		szRowSize += record.uLength;

    manipulate(record);
		szLargestFieldSize = std::max(szLargestFieldSize, static_cast<size_t>(record.uLength));
	}

  for (auto &&h : vecRecords)
  {
    int firstZero = ToHeaderName(h.archName);
    columnNames.push_back(std::string(h.archName, firstZero));
  }
}

void DbfFile_c::DumpAll(const char *szDestFileName)
{
	std::ofstream out(szDestFileName);

	std::vector<char> vecBuffer;
	vecBuffer.resize(szLargestFieldSize);

	size_t uTotalBytes = 0;
	size_t uNumRecords = 0;
	while(!clFile.eof())
	{
		char deleted;
		clFile.read(&deleted, 1);		
		if(deleted == 0x2A)
		{
			clFile.seekg(szRowSize, std::ios_base::cur);
			continue;
		}
    std::map<std::string, std::string> entry;
    for (size_t i = 0; i < vecRecords.size(); ++i)
    {
      DbfRecord_s &record = vecRecords[i];

      clFile.read(&vecBuffer[0], record.uLength);
      int lastLetter = record.uLength;
      int firstLetter = 0;
      for (int i = record.uLength - 1; i >= 0; i--)
      {
        if (vecBuffer[i] != ' ' && vecBuffer[i] != '\0')
        {
          lastLetter = i;
          break;
        }
      }
      for (int i = 0; i < lastLetter; i++)
      {
        if (vecBuffer[i] != ' ')
        {
          firstLetter = i;
          break;
        }
      }
      int firstHeaderZero = ToHeaderName(record.archName);
      entry[std::string(record.archName, firstHeaderZero)] = std::string(&vecBuffer[firstLetter], lastLetter - firstLetter + 1);
			out.write(&vecBuffer[0], record.uLength);
			uTotalBytes += record.uLength;
		}
    data.push_back(entry);
		++uNumRecords;
		++uTotalBytes;

		out << std::endl;
	}

	std::cout << "Created " << uNumRecords << ", records " << uTotalBytes << " bytes." << std::endl;
}