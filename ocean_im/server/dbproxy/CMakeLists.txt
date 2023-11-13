project(dbproxy)

set(BRPC_INCLUDE_PATH "thirds/brpc1.7/include")
set(BRPC_LIB "thirds/brpc1.7/lib/libbrpc.a")

set(SOCI_INCLUDE_PATH "thirds/soci4.0.3/include")
set(SOCI_LIBRARY "thirds/soci4.0.3/lib")

find_path(BRPC_INCLUDE_PATH NAMES brpc/server.h)
if(LINK_SO)
  find_library(BRPC_LIB NAMES brpc)
else()
  find_library(BRPC_LIB NAMES libbrpc.a brpc)
endif()

if((NOT BRPC_INCLUDE_PATH) or (NOT BRPC_LIB))
  message(FATAL_ERROR "fail to find brpc")
endif()

add_executable(${PROJECT_NAME} dbproxy.cpp dbproxy_servce.cpp dbproxy_servce.h)

target_include_directories(
  ${PROJECT_NAME}
  PRIVATE ${CMAKE_SOURCE_DIR}/oceanim
          ${CMAKE_SOURCE_DIR}/oceanim/server
          ${CMAKE_BINARY_DIR}/oceanim/server
          ${CMAKE_BINARY_DIR}/oceanim/server/access
          ${CMAKE_CURRENT_BINARY_DIR}
          ${BRPC_INCLUDE_PATH}
          ${SOCI_INCLUDE_PATH}
          ${GFLAGS_INCLUDE_PATH})

target_link_libraries(
  ${PROJECT_NAME}
  PRIVATE ${BRPC_LIB}
          ${PROTOBUF_LIBRARIES}
          ${GFLAGS_LIBRARIES}
          OpenSSL::SSL
          glog::glog
          leveldb::leveldb
          oceanim::proto
          ${SOCI_LIBRARY}
          dl)
