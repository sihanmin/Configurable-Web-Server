cmake_minimum_required(VERSION 3.10.0)
project(aptx)

# Make sure developers do not run cmake in the main project directory, to keep
# build artifacts from becoming clutter
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "In-source builds not allowed.
    Please make a new directory (called a build directory) and run CMake from there.
    You may need to remove CMakeCache.txt." )
endif()

# Turn on debug builds if we are building in a devel shell
if (CMAKE_BUILD_TYPE STREQUAL "" AND "$ENV{DEVEL_SHELL}" STREQUAL "1")
    message(STATUS "Setting debug build type by default in devel shell")
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Output binaries to a sub directory "bin"
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Enable cmake testing
include(CTest)
enable_testing()

# Enable GoogleTest
include(GoogleTest)
add_subdirectory(/usr/src/googletest googletest)

# Enable Boost
# Use static libraries so binaries can be deployed without a full boost install
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.50 REQUIRED COMPONENTS log_setup log system)
message(STATUS "Boost version: ${Boost_VERSION}")

include_directories(include)

# TODO(aptx): Update name and srcs
add_library(request_parser_lib src/request_parser.cc src/reply.cc)
add_library(reply_lib src/reply.cc)
add_library(session_lib src/session.cc)
set_property(TARGET session_lib PROPERTY CXX_STANDARD 17)
add_library(request_handler_lib src/echo_handler.cc src/bad_request_handler.cc src/static_handler.cc src/not_found_handler.cc src/status_handler.cc src/mime_types.cc src/dispatcher.cc src/proxy_handler.cc src/health_handler.cc src/game_handler.cc src/utilities.cc src/base64.cc)
set_property(TARGET request_handler_lib PROPERTY CXX_STANDARD 17)
add_library(config_parser_lib src/config_parser.cc)
add_library(server_lib src/server.cc)
set_property(TARGET server_lib PROPERTY CXX_STANDARD 17)
target_link_libraries(reply_lib Boost::system)
add_library(pic_modifier src/pic_modifier.cc)
target_link_libraries(pic_modifier png jpeg)
target_link_libraries(request_handler_lib pic_modifier)


# TODO(aptx): Update executable name, srcs, and deps
add_executable(server src/server_main.cc)
set_property(TARGET server PROPERTY CXX_STANDARD 17)
target_link_libraries(server server_lib session_lib reply_lib request_parser_lib request_handler_lib config_parser_lib pic_modifier Boost::system Boost::log_setup Boost::log)



# TODO(aptx): Update test executable name, srcs, and deps
add_executable(config_parser_test tests/config_parser_test.cc)
target_link_libraries(config_parser_test config_parser_lib gtest_main gmock_main)
add_executable(reply_test tests/reply_test.cc)
target_link_libraries(reply_test reply_lib gtest_main gmock_main)
add_executable(request_parser_test tests/request_parser_test.cc)
target_link_libraries(request_parser_test request_parser_lib gtest_main gmock_main)
add_executable(dispatcher_test tests/dispatcher_test.cc)
set_property(TARGET dispatcher_test PROPERTY CXX_STANDARD 17)
target_link_libraries(dispatcher_test config_parser_lib request_handler_lib reply_lib Boost::system Boost::log_setup Boost::log gtest_main)
add_executable(request_handler_test tests/request_handler_test.cc)
set_property(TARGET request_handler_test PROPERTY CXX_STANDARD 17)
target_link_libraries(request_handler_test config_parser_lib request_handler_lib reply_lib gtest_main Boost::system Boost::log_setup Boost::log)
add_executable(session_test tests/session_test.cc)
set_property(TARGET session_test PROPERTY CXX_STANDARD 17)
target_link_libraries(session_test session_lib config_parser_lib request_parser_lib request_handler_lib gtest_main gmock_main Boost::system Boost::log_setup Boost::log)
add_executable(server_test tests/server_test.cc)
set_property(TARGET server_test PROPERTY CXX_STANDARD 17)
target_link_libraries(server_test server_lib session_lib request_parser_lib request_handler_lib config_parser_lib gtest_main gmock_main Boost::system Boost::log_setup Boost::log)
add_executable(pic_modifier_test tests/pic_modifier_test.cc)
target_link_libraries(pic_modifier_test pic_modifier gtest_main)


# TODO(aptx): Update with test binary
gtest_discover_tests(config_parser_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(reply_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(request_parser_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(dispatcher_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(request_handler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(session_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(server_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(pic_modifier_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
add_test(NAME integration_test COMMAND ${BASH_PROGRAM} ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration_test.sh)
add_test(NAME multithreading_test COMMAND ${BASH_PROGRAM} ${CMAKE_CURRENT_SOURCE_DIR}/tests/multithreading_test.sh)

# TODO(aptx): as TESTS become available, add more TARGETS and TESTS in the coverage report
include(cmake/CodeCoverageReportConfig.cmake)
generate_coverage_report(TARGETS server config_parser_lib reply_lib request_parser_lib request_handler_lib session_lib server_lib TESTS config_parser_test reply_test request_parser_test dispatcher_test request_handler_test session_test server_test pic_modifier_test)
