cmake_minimum_required(VERSION 3.1)

if(NOT CMAKE_DEBUG_POSTFIX)
  set(CMAKE_DEBUG_POSTFIX d)
endif()

project(FAKT_project CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(QT_DIR "" CACHE PATH "Path to QT")
include_directories(${QT_DIR}/include)
link_directories(${QT_DIR}/lib)

set(DBF_DIR "" CACHE PATH "Path to DBF Library")
link_directories(${DBF_DIR}/lib)

set(GTEST_DIR "" CACHE FILEPATH "path to gtest dir")
link_directories(${GTEST_DIR}/lib)

find_package(Qt5Widgets)

include_directories(include)

set(SRCS
  src/adding_pages.cpp
  src/address.cpp
  src/basetab.cpp
  src/backup.cpp
  src/export.cpp
  src/general_print_page.cpp
  src/invoice.cpp
  src/jobsite.cpp
  src/fakt.cpp
  src/log.cpp
  src/main.cpp
  src/material.cpp
  src/offer.cpp
  src/service.cpp
  src/single_entry.cpp
  src/utils.cpp
)

set(INCS
  include/adding_pages.h
  include/address.h
  include/basetab.h
  include/backup.h
  include/data_entries.h
  include/export.h
  include/fakt.h
  include/general_print_page.h
  include/invoice.h
  include/jobsite.h
  include/log.h
  include/material.h
  include/offer.h
  include/overwatch.h
  include/service.h
  include/single_entry.h
  include/sql_helper.hpp
  include/sql_helper_impl.hpp
  include/utils.h
)

QT5_WRAP_UI(FORMS
  form/address_page.ui
  form/basetab.ui
  form/fakt.ui
  form/general_page.ui
  form/material_page.ui
  form/general_main_page.ui
  form/general_print_page.ui
  form/service_page.ui
)

add_executable(FAKT ${INCS} ${SRCS} ${FORMS})
target_link_libraries(FAKT QT5Cored QT5Widgetsd QT5Sqld Qt5Guid Qt5PrintSupportd)

add_subdirectory(tools)

add_subdirectory(test)