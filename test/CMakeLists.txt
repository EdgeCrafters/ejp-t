cmake_minimum_required(VERSION 3.12)
project(
	EJS-T-UNIT-TEST
	)

include_directories(../lib)

add_executable(test-main main.c )

target_link_libraries( test-main PRIVATE parse sha256 diff http info cJSON common)
target_compile_options( ejs-t PUBLIC -w)
