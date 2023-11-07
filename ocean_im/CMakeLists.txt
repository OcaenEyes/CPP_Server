cmake_minimum_required(VERSION 3.16)
project(oceanim)

file(GLOB SRC_FILE "client/" "server/" "thirds/" "util/")

set(THIRD_DIR "thirds")
set(BRPC_DIR ${THIRD_DIR}/brpc1.7)
include_directories(${BRPC_DIR}/include)
link_libraries(${BRPC_DIR}/lib)

