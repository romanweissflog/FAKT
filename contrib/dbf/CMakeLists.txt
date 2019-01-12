cmake_minimum_required(VERSION 3.1)

project(DBF CXX C)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include_directories(include)

if(NOT CMAKE_DEBUG_POSTFIX)
  set(CMAKE_DEBUG_POSTFIX d)
endif()

set(SRCS
  src/dbf.c
  src/dbf.inl
  src/ioapi.c
)

set(INCS
  include/dbf.hpp
  include/dbf.h
  include/bool.h
  include/ioapi.h
  include/zconf.h
  include/zlib.h
  include/stdint-ms.h
)

add_library(DBF ${INCS} ${SRCS})
install(TARGETS DBF DESTINATION lib)
install(FILES ${INCS} DESTINATION include)
