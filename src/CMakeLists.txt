cmake_minimum_required(VERSION 3.11)

include_directories(../lib)

add_executable(ejs-t main.c repo.c workbook.c)

target_link_libraries( ejs-t PUBLIC parse sha256 diff http info)
target_compile_options( ejs-t PUBLIC -w)
