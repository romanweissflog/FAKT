cmake_minimum_required(VERSION 3.1)

project(FAKT_project CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(QT_DIR "" CACHE PATH "Path to QT")
include_directories(${QT_DIR}/include)
link_directories(${QT_DIR}/lib)

find_package(Qt5Widgets)

include_directories(include)

set(SRCS
  src/main.cpp
  src/fakt.cpp
  src/material.cpp
  src/service.cpp
  src/invoice.cpp
  src/single_invoice.cpp
  src/address.cpp
  src/utils.cpp
  src/basetab.cpp
  src/adding_pages.cpp
  src/log.cpp
)

set(INCS
  include/fakt.h
  include/material.h
  include/service.h
  include/invoice.h
  include/single_invoice.h
  include/address.h
  include/utils.h
  include/basetab.h
  include/adding_pages.h
  include/data_entries.h
  include/overwatch.h
  include/sql_helper.hpp
  include/sql_helper_impl.hpp
  include/log.h
)

QT5_WRAP_UI(FORMS
  form/fakt.ui
  form/basetab.ui
  form/service_page.ui
  form/material_page.ui
  form/address_page.ui
  form/general_page.ui
  form/invoice_page.ui
)

add_executable(FAKT ${INCS} ${SRCS} ${FORMS})
target_link_libraries(FAKT QT5Cored QT5Widgetsd QT5Sqld Qt5Guid Qt5PrintSupportd)

add_subdirectory(tools)