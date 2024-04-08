cmake_minimum_required(VERSION 3.0.0)
project(tcp-client VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

# Check for socket support
include(CheckSymbolExists)
check_symbol_exists(socket "sys/socket.h" HAVE_SOCKET)

add_executable(tcp-client main.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
