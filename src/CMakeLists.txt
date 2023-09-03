add_executable(ejp-t main.c workbook.c workbook.service.c manage.c manage.service.c)
target_link_libraries(ejp-t common parse cJSON http sha256 info)
target_include_directories(ejp-t PRIVATE ${CMAKE_SOURCE_DIR}/include)
