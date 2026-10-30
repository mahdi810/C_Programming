cmake_minimum_required(VERSION 3.15)

# making a static library from all the header files 
set(LIB_FILES
    ${CMAKE_SOURCE_DIR}/src/header1.c
    # add all the file belonging to the headers.
)
add_library(my_project_lib STATIC ${LIB_FILES})
add_executable(my_project_lib PUBLIC ${LIB_FILES})
target_include_directories(my_project_lib PRIVATE ${CMAKE_SOURCE_DIR}/include)