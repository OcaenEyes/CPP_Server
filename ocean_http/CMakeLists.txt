cmake_minimum_required(VERSION 3.20)
project(oceanhttp)

include_directories(${PROJECT_SOURCE_DIR})

add_executable(
  oceanhttp
  main.cpp
  timer/lst_timer.h
  timer/lst_timer.cpp
  http/http_conn.h
  http/http_conn.cpp
  log/block_queue.h
  lock/locker.h
  lock/locker.cpp
  threadpool/threadpool.h
  log/log.h
  log/log.cpp
  cgimysql/sql_connection_pool.h
  cgimysql/sql_connection_pool.cpp
  webserver.h
  webserver.cpp
  config.cpp
  config.h)

target_link_libraries(oceanhttp pthread)
target_link_libraries(oceanhttp mysqlclient)
