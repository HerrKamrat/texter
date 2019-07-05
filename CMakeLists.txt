cmake_minimum_required(VERSION 2.8)

set(UTF8_SRC_DIR "${CMAKE_SOURCE_DIR}/utf8_v2_3_4/source/")
set(UNIBREAK_SRC_DIR "${CMAKE_SOURCE_DIR}/libunibreak-4.1/src/")

set(HB_DIR "${CMAKE_SOURCE_DIR}/harfbuzz-2.3.1/")
set(HB_LIB_DIR "${HB_DIR}/osx/")
set(HB_INCLUDE_DIR "${HB_DIR}/src/")

set(FT_DIR "${CMAKE_SOURCE_DIR}/freetype-2.10.0/")
set(FT_LIB_DIR "${FT_DIR}/osx/")
set(FT_INCLUDE_DIR "${FT_DIR}/osx/include/")

#test
set(STB_SRC_DIR "${CMAKE_SOURCE_DIR}/stb/")


set(CMAKE_CXX_STANDARD 20)

project(texter)
#add_library(utf8  
#utf8_v2_3_4/source/utf8.h
#utf8_v2_3_4/source/utf8/checked.h
#utf8_v2_3_4/source/utf8/core.h
#utf8_v2_3_4/source/utf8/unchecked.h
#)

add_executable(texter 
main.cpp
text.cpp
text.hpp
escape.hpp
escape.cpp

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

${STB_SRC_DIR}/stb_image_write.h

)
include_directories(texter 
${UTF8_SRC_DIR}
${UNIBREAK_SRC_DIR}
${HB_INCLUDE_DIR}
${FT_INCLUDE_DIR}

${STB_SRC_DIR}
)

target_link_libraries(texter bz2 z ${HB_LIB_DIR}/libharfbuzz.a ${FT_LIB_DIR}/libfreetype.a)

#install(TARGETS texter DESTINATION bin)
