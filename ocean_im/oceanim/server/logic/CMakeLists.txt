project(logic)

set(CMAKE_CXX_STANDARD 11)

get_filename_component(TMPDIR1 ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
get_filename_component(TMPDIR2 ${TMPDIR1} DIRECTORY)
get_filename_component(PARENT_DIR ${TMPDIR2} DIRECTORY)

set(BRPC_DIR ${PARENT_DIR}/thirds/brpc1.7)
find_library(BRPC_LIB NAMES libbrpc.a  PATHS ${BRPC_DIR}/lib NO_DEFAULT_PATH)
set(BRPC_INCLUDE_PATH ${BRPC_DIR}/include)

set(SOCI_DIR ${PARENT_DIR}/thirds/soci4.0.3)
find_library(SOCI_LIB NAMES libsoci_core.a  PATHS ${SOCI_DIR}/lib NO_DEFAULT_PATH)
set(SOCI_INCLUDE_PATH ${SOCI_DIR}/include)

add_executable(${PROJECT_NAME} logic.cpp logic_service.cpp logic_service.h)

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
          ${SOCI_LIB}
          dl)
