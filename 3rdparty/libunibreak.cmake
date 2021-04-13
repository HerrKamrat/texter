cmake_minimum_required(VERSION 2.8)

project(libunibreak)

set(UNIBREAK_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/libunibreak-4.1/src/")

add_library(${PROJECT_NAME}

${UNIBREAK_SRC_DIR}/wordbreakdef.h
${UNIBREAK_SRC_DIR}/emojidef.c
${UNIBREAK_SRC_DIR}/emojidef.h
${UNIBREAK_SRC_DIR}/graphemebreak.c
${UNIBREAK_SRC_DIR}/graphemebreak.h
${UNIBREAK_SRC_DIR}/graphemebreakdef.h
${UNIBREAK_SRC_DIR}/linebreak.c
${UNIBREAK_SRC_DIR}/linebreak.h
${UNIBREAK_SRC_DIR}/linebreakdata.c
${UNIBREAK_SRC_DIR}/linebreakdef.c
${UNIBREAK_SRC_DIR}/linebreakdef.h
${UNIBREAK_SRC_DIR}/unibreakbase.c
${UNIBREAK_SRC_DIR}/unibreakbase.h
${UNIBREAK_SRC_DIR}/unibreakdef.c
${UNIBREAK_SRC_DIR}/unibreakdef.h
${UNIBREAK_SRC_DIR}/wordbreak.c
${UNIBREAK_SRC_DIR}/wordbreak.h
${UNIBREAK_SRC_DIR}/wordbreakdata.c

)

target_include_directories(${PROJECT_NAME} PUBLIC "${UNIBREAK_SRC_DIR}")
