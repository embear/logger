# include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# test target
add_custom_target(test ${CMAKE_CURRENT_SOURCE_DIR}/test.rb)

# sources
set(test_src main.c)

# destination
add_executable(logger_test ${test_src})

# library
target_link_libraries(logger_test logger)
