add_library(sqlite3 sqlite3.h sqlite3.c)
target_compile_options(sqlite3 PRIVATE /TC)
SET_SOURCE_FILES_PROPERTIES(sqlite3.c PROPERTIES LANGUAGE C) 

add_executable(dbfToDatabase DbfFile.h DbfFile.cpp dbfToDb.cpp)
target_link_libraries(dbfToDatabase sqlite3)

add_executable(invoiceToDb DbfFile.h DbfFile.cpp invoiceToDb.cpp)
target_link_libraries(invoiceToDb sqlite3)