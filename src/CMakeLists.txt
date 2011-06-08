# include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# sources
set(lib_src logger.c)

# destination
add_library(logger ${lib_src})

# install target
install(TARGETS logger DESTINATION lib)
