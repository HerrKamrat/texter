cmake_minimum_required(VERSION 2.8)

project(utf8)

set(UTF8_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/utf8_v2_3_4/source/")

add_library(${PROJECT_NAME} INTERFACE)

target_include_directories(${PROJECT_NAME} INTERFACE "${UTF8_SRC_DIR}")
