cmake_minimum_required(VERSION 3.16)
project(oceanim)

message("PROJECT_BINARY_DIR: ${PROJECT_BINARY_DIR}")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

set(CMAKE_MODULE_PATH  ${PROJECT_SOURCE_DIR}/cmake)


message("CMAKE_MODULE_PATH:${CMAKE_MODULE_PATH}")

set(Glog_DIR "/usr/local/lib/cmake/glog")

set(glags_DIR "/usr/local/lib/cmake/gflags")

find_package(glog REQUIRED)
find_package(Protobuf REQUIRED)
find_package(gflags REQUIRED)
find_package(leveldb REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(MySQL REQUIRED)
# find_package(Soci REQUIRED)

message("project_source_dir: ${PROJECT_SOURCE_DIR}")
message("cmake_current_binary_dir: ${CMAKE_CURRENT_BINARY_DIR}")

include(CompileProto)
set(PROTO_FILES common/message.proto 
                access/access.proto 
                idgen/idegen.proto
                logic/logic.proto 
                dbproxy/dbproxy.proto)
                
file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/oceanim/server)
set(PROTO_FLAGS ${PROTO_FLAGS} -I${PROTOBUF_INCLUDE_DIR})
compile_proto(
  PROTO_HDRS PROTO_SRCS ${PROJECT_BINARY_DIR}/oceanim/server
  ${PROJECT_BINARY_DIR}/oceanim/server ${PROJECT_SOURCE_DIR}/oceanim/server
  "${PROTO_FILES}")

add_library(oceanim_proto_lib OBJECT ${PROTO_SRCS} ${PROTO_HDRS})
add_library(oceanim::proto ALIAS oceanim_proto_lib)

target_include_directories(oceanim_proto_lib
                           PRIVATE ${PROJECT_BINARY_DIR}/oceanim/server)

add_subdirectory(client)
add_subdirectory(server/access)
add_subdirectory(server/idgen)

add_subdirectory(server/dbproxy)
add_subdirectory(server/logic)