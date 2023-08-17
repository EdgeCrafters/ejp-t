add_library(cJSON cJSON.c)
target_include_directories(cJSON PUBLIC ${CMAKE_SOURCE_DIR}/include)


add_library(http http.c)
target_link_libraries(http libcurl)
target_include_directories(http PUBLIC ${CMAKE_SOURCE_DIR}/include)

add_library(common common.c)
target_link_libraries(common http)
target_include_directories(common PUBLIC ${CMAKE_SOURCE_DIR}/include)

add_library(sha256 sha256.c)
target_include_directories(sha256 PUBLIC ${CMAKE_SOURCE_DIR}/include)

add_library(info info.c)
target_include_directories(info PUBLIC ${CMAKE_SOURCE_DIR}/include)

add_library(parse parse.c)
target_link_libraries(parse common)
target_include_directories(parse PUBLIC ${CMAKE_SOURCE_DIR}/include)
