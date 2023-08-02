project(EJS-T-UTIL)

add_library(parse STATIC parse.c)
add_library(sha256 STATIC sha256.c)
add_library(diff STATIC diff.c)
add_library(http STATIC http.c)
add_library(info STATIC info.c)
add_library(cJSON STATIC cJSON.c)
add_library(common STATIC common.c)

target_link_libraries(http PRIVATE libcurl)
target_link_libraries(diff PRIVATE libgit2)
target_link_libraries(common PRIVATE http)

target_compile_options(http PUBLIC -w)
target_compile_options(diff PUBLIC -w)

target_include_directories(parse PUBLIC ${CMAKE_SOURCE_DIR}/includes)
target_include_directories(sha256 PUBLIC ${CMAKE_SOURCE_DIR}/includes)
target_include_directories(diff PUBLIC ${CMAKE_SOURCE_DIR}/includes)
target_include_directories(http PUBLIC ${CMAKE_SOURCE_DIR}/includes)
target_include_directories(info PUBLIC ${CMAKE_SOURCE_DIR}/includes)
target_include_directories(cJSON PUBLIC ${CMAKE_SOURCE_DIR}/includes)
target_include_directories(common PUBLIC ${CMAKE_SOURCE_DIR}/includes)
