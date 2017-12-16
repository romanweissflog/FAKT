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
  src/adding_pages.cpp
  src/address.cpp
  src/basetab.cpp
  src/export.cpp
  src/invoice.cpp
  src/jobsite.cpp
  src/fakt.cpp
  src/log.cpp
  src/main.cpp
  src/material.cpp
  src/offer.cpp
  src/service.cpp
  src/single_invoice.cpp
  src/single_offer.cpp
  src/utils.cpp
)

set(INCS
  include/adding_pages.h
  include/address.h
  include/basetab.h
  include/data_entries.h
  include/export.h
  include/fakt.h
  include/invoice.h
  include/jobsite.h
  include/log.h
  include/material.h
  include/offer.h
  include/overwatch.h
  include/service.h
  include/single_offer.h
  include/single_invoice.h
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
  form/service_page.ui
)

add_executable(FAKT ${INCS} ${SRCS} ${FORMS})
target_link_libraries(FAKT QT5Cored QT5Widgetsd QT5Sqld Qt5Guid Qt5PrintSupportd)

add_subdirectory(tools)