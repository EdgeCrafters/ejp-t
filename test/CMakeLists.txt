add_executable(test-main main.c)

target_link_libraries( test-main parse sha256 git http info cJSON common)
target_include_directories( test-main  PRIVATE ${CMAKE_SOURCE_DIR}/include)