cmake_minimum_required(VERSION 3.16)
project(oceanhttp)

include_directories(${PROJECT_SOURCE_DIR} NO_DEFAULT_PATH)

add_executable(
  oceanhttp
  main.cpp
  config.cpp
  config.h
  webserver.cpp
  timer/lst_timer.cpp
  cgimysql/sql_connection_pool.cpp
  http/http_conn.cpp
  lock/locker.cpp
  log/log.cpp
  log/block_queue.cpp
  root/
  threadpool/threadpool.cpp)

target_link_libraries(oceanhttp pthread)
target_link_libraries(oceanhttp mysqlclient)
