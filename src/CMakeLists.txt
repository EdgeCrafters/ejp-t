add_executable(ejs-t main.c workbook.c workbook.service.c)
target_link_libraries(ejs-t common git parse cJSON http sha256 info)
target_include_directories(ejs-t PRIVATE ${CMAKE_SOURCE_DIR}/include)
