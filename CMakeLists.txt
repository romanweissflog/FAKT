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

find_package(Qt5Widgets)

include_directories(include)

set(TAB_SRCS
  src/tabs/address.cpp
  src/tabs/basetab.cpp
  src/tabs/invoice.cpp
  src/tabs/jobsite.cpp
  src/tabs/main_tab.cpp
  src/tabs/material.cpp
  src/tabs/offer.cpp
  src/tabs/payment.cpp
  src/tabs/service.cpp
)
source_group("Source Files\\tabs" FILES ${TAB_SRCS})

set(TAB_INCS
  include/tabs/address.h
  include/tabs/basetab.h
  include/tabs/invoice.h
  include/tabs/jobsite.h
  include/tabs/main_tab.h
  include/tabs/material.h
  include/tabs/offer.h
  include/tabs/payment.h
  include/tabs/service.h
)
source_group("Header Files\\tabs" FILES ${TAB_INCS})

set(PAGE_SRCS
  src/pages/address_page.cpp
  src/pages/general_main_page.cpp
  src/pages/general_page.cpp
  src/pages/general_print_page.cpp
  src/pages/invoice_page.cpp
  src/pages/material_page.cpp
  src/pages/offer_page.cpp
  src/pages/page_framework.cpp
  src/pages/parent_page.cpp
  src/pages/payment_page.cpp
  src/pages/service_page.cpp
  src/pages/single_entry.cpp
  src/pages/single_invoice.cpp
  src/pages/single_jobsite.cpp
  src/pages/single_offer.cpp
  src/pages/summary_page.cpp
)
source_group("Source Files\\pages" FILES ${PAGE_SRCS})

set(PAGE_INCS
  include/pages/address_page.h
  include/pages/general_main_page.h
  include/pages/general_page.h
  include/pages/general_print_page.h
  include/pages/invoice_page.h
  include/pages/material_page.h
  include/pages/offer_page.h
  include/pages/page_framework.h
  include/pages/parent_page.h
  include/pages/payment_page.h
  include/pages/service_page.h
  include/pages/single_entry.h
  include/pages/single_invoice.h
  include/pages/single_jobsite.h
  include/pages/single_offer.h
  include/pages/summary_page.h
)
source_group("Header Files\\pages" FILES ${PAGE_INCS})

set(FUNCTION_SRCS
  src/functionality/backup.cpp
  src/functionality/export.cpp
  src/functionality/fakt.cpp
  src/functionality/log.cpp
  src/functionality/utils.cpp
)
source_group("Source Files\\functionality" FILES ${FUNCTION_SRCS})

set(FUNCTION_INCS
  include/functionality/backup.h
  include/functionality/data_entries.h
  include/functionality/defines.h
  include/functionality/export.h
  include/functionality/fakt.h
  include/functionality/log.h
  include/functionality/overwatch.h
  include/functionality/sql_helper.hpp
  include/functionality/sql_helper_impl.hpp
  include/functionality/utils.h
  include/functionality/utils.hpp
)
source_group("Header Files\\functionality" FILES ${FUNCTION_INCS})

set(SRCS
  src/main.cpp
  ${TAB_SRCS}
  ${PAGE_SRCS}
  ${FUNCTION_SRCS}
)

set(INCS
  ${TAB_INCS}
  ${PAGE_INCS}
  ${FUNCTION_INCS}
)

QT5_WRAP_UI(FORMS
  form/address_content.ui
  form/basetab.ui
  form/fakt.ui
  form/general_content.ui
  form/material_content.ui
  form/general_main_content.ui
  form/general_print_page.ui
  form/main_tab.ui
  form/page_framework.ui
  form/payment_content.ui
  form/service_content.ui
  form/summary_page.ui
)

add_executable(FAKT WIN32 ${INCS} ${SRCS} ${FORMS})
target_link_libraries(FAKT 
  debug QT5Cored debug QT5Widgetsd 
  debug QT5Sqld debug Qt5Guid 
  debug Qt5PrintSupportd
  optimized QT5Core optimized QT5Widgets 
  optimized QT5Sql optimized Qt5Gui 
  optimized Qt5PrintSupport
)
set_target_properties(FAKT PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
set_target_properties(FAKT PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE")

install(TARGETS FAKT CONFIGURATIONS Debug DESTINATION debug/bin)
install(FILES 
  ${CMAKE_CURRENT_BINARY_DIR}/Debug/Qt5Cored.dll 
  ${CMAKE_CURRENT_BINARY_DIR}/Debug/Qt5Guid.dll 
  ${CMAKE_CURRENT_BINARY_DIR}/Debug/Qt5PrintSupportd.dll 
  ${CMAKE_CURRENT_BINARY_DIR}/Debug/Qt5Sqld.dll 
  ${CMAKE_CURRENT_BINARY_DIR}/Debug/Qt5Widgetsd.dll 
  CONFIGURATIONS Debug DESTINATION debug/bin)
install(TARGETS FAKT CONFIGURATIONS Release DESTINATION bin)
install(FILES 
  ${CMAKE_CURRENT_BINARY_DIR}/Release/Qt5Core.dll 
  ${CMAKE_CURRENT_BINARY_DIR}/Release/Qt5Gui.dll 
  ${CMAKE_CURRENT_BINARY_DIR}/Release/Qt5PrintSupport.dll 
  ${CMAKE_CURRENT_BINARY_DIR}/Release/Qt5Sql.dll 
  ${CMAKE_CURRENT_BINARY_DIR}/Release/Qt5Widgets.dll 
  CONFIGURATIONS Release DESTINATION bin)
install(DIRECTORY scripts DESTINATION /)
install(FILES res/logo.png res/splash.jpeg DESTINATION bin)
install(FILES settings.ini logFile.log DESTINATION bin)

add_subdirectory(tools)
