add_library(parse STATIC parse.c)
add_library(sha256 STATIC sha256.c)
add_library(diff STATIC diff.c)

target_link_libraries(diff PRIVATE sha256 git2)
target_link_libraries(parse PRIVATE sha256 libgit2 libcurl)

target_include_directories(parse PUBLIC ${CMAKE_SOURCE_DIR}/includes)
target_include_directories(sha256 PUBLIC ${CMAKE_SOURCE_DIR}/includes)
target_include_directories(diff PUBLIC ${CMAKE_SOURCE_DIR}/includes)
