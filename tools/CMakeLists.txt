set(name sqlite3)
add_library(${name} ${name}.h ${name}.c)
target_compile_options(${name} PRIVATE /TC)
set_target_properties(${name} PROPERTIES FOLDER Libs)
SET_SOURCE_FILES_PROPERTIES(${name}.c PROPERTIES LANGUAGE C) 

set(name dbfToDatabase)
add_executable(${name} utf_8.h util.h util.cpp dbfToDb.cpp)
target_include_directories(${name} PUBLIC ${DBF_DIR}/include)
target_link_libraries(${name} sqlite3)
target_link_libraries(${name} debug DBFd.lib optimized DBF.lib)
set_target_properties(${name} PROPERTIES FOLDER Tools)

set(name create_extra_database)
add_executable(${name} ${name}.cpp)
target_include_directories(${name} PUBLIC)
target_link_libraries(${name} sqlite3 
  debug Qt5Cored debug Qt5Widgetsd 
  debug Qt5Sqld debug Qt5Guid 
  optimized Qt5Core optimized Qt5Widgets 
  optimized Qt5Sql optimized Qt5Gui)
set_target_properties(${name} PROPERTIES FOLDER Tools)

set(name invoiceAndOfferToDb)
add_executable(${name} utf_8.h util.h util.cpp ${name}.cpp)
target_include_directories(${name} PUBLIC ${DBF_DIR}/include)
target_link_libraries(${name} sqlite3)
target_link_libraries(${name} debug DBFd.lib optimized DBF.lib)
set_target_properties(${name} PROPERTIES FOLDER Tools)

install(TARGETS 
    dbfToDatabase invoiceAndOfferToDb create_extra_database
    CONFIGURATIONS Debug DESTINATION debug/tools)
install(TARGETS 
    dbfToDatabase invoiceAndOfferToDb create_extra_database
    CONFIGURATIONS Release DESTINATION tools)