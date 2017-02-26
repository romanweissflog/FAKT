cmake_minimum_required(VERSION 3.2)

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
  src/leistung.cpp
  src/rechnung.cpp
  src/utils.cpp
  src/basetab.cpp
  src/adding_pages.cpp
)

set(INCS
  include/fakt.h
  include/material.h
  include/leistung.h
  include/rechnung.h
  include/utils.h
  include/basetab.h
  include/adding_pages.h
  include/data_entries.h
)

QT5_WRAP_UI(FORMS
  form/fakt.ui
  form/basetab.ui
  form/leistung_page.ui
)

add_executable(FAKT ${INCS} ${SRCS} ${FORMS})
target_link_libraries(FAKT QT5Cored QT5Widgetsd QT5Sqld)

add_subdirectory(tools)