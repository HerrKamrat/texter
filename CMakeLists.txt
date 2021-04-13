cmake_minimum_required(VERSION 2.8)


#set(HB_HAVE_FREETYPE ON CACHE INTERNAL "")
#set(FREETYPE_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/freetype-2.10.0/include/")
#set(FREETYPE_LIBRARY "freetype")

#set(FT_WITH_HARFBUZZ ON CACHE INTERNAL "")
#set(FT_CONFIG_OPTION_ERROR_STRINGS ON CACHE INTERNAL "")

#set(HARFBUZZ_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/harfbuzz-2.3.1/src/")
#set(HARFBUZZ_LIBRARIES "harfbuzz")

#add_subdirectory("${CMAKE_SOURCE_DIR}/freetype-2.10.0")
#add_library(Freetype::Freetype ALIAS freetype)

#add_subdirectory("${CMAKE_SOURCE_DIR}/harfbuzz-2.3.1")

add_subdirectory("${CMAKE_SOURCE_DIR}/3rdparty")


set(HB_DIR "${CMAKE_SOURCE_DIR}/harfbuzz-2.3.1/")
set(HB_LIB_DIR "${HB_DIR}/osx/")
set(HB_INCLUDE_DIR "${HB_DIR}/src/")

set(FT_DIR "${CMAKE_SOURCE_DIR}/freetype-2.10.0/")

set(FT_LIB_DIR "${FT_DIR}/osx/")
set(FT_INCLUDE_DIR "${FT_DIR}/osx/include/")

set(BX_DIR "${CMAKE_SOURCE_DIR}/3rdparty/bx/")
set(BX_INCLUDE_DIR "${BX_DIR}/include/")

set(HB_LIB_DIR "${HB_DIR}/build/Debug/")

set(FT_DIR "${CMAKE_SOURCE_DIR}/freetype-2.10.0/")
set(FT_LIB_DIR "${FT_DIR}/build/Debug/")
set(FT_INCLUDE_DIR "${FT_DIR}/include/")

set(STB_SRC_DIR "${CMAKE_SOURCE_DIR}/3rdparty/stb/")

set(CMAKE_CXX_STANDARD 17)

project(texter)

add_executable(texter 
main.cpp
text.cpp
text.hpp
escape.hpp
escape.cpp


${STB_SRC_DIR}/stb_image_write.h

${BX_DIR}/src/amalgamated.cpp

)

include_directories(texter 
#${UTF8_SRC_DIR}
#${UNIBREAK_SRC_DIR}
${HB_INCLUDE_DIR}
${FT_INCLUDE_DIR}

${STB_SRC_DIR}

${BX_INCLUDE_DIR}/compat/msvc
${BX_INCLUDE_DIR}
${BX_DIR}/3rdparty/
)

#target_link_libraries(texter bz2 z ${HB_LIB_DIR}/libharfbuzz.a ${FT_LIB_DIR}/libfreetype.a)
#target_link_libraries(texter ${HB_LIB_DIR}/harfbuzz.lib ${FT_LIB_DIR}/freetyped.lib)

target_link_libraries(texter sheenbidi)
target_link_libraries(texter libunibreak)
target_link_libraries(texter utf8)
target_link_libraries(texter nlohmann_json::nlohmann_json)
target_link_libraries(texter freetype)
target_link_libraries(texter harfbuzz)
target_link_libraries(texter msdfgen::msdfgen msdfgen::msdfgen-ext)

#install(TARGETS texter DESTINATION bin)
add_compile_definitions(texter
    __STDC_LIMIT_MACROS
    __STDC_CONSTANT_MACROS
    __STDC_FORMAT_MACROS
)
