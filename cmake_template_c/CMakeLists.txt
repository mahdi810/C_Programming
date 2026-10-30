cmake_minimum_required(VERSION 3.15)
project(my_project C)

set(NATIVE_C_BUILD  "For building application for windows" ON)
set(ENABLE_TESTS "For running test in the unity testing framework" ON)

if(NATIVE_C_BUILD)
    add_subdirectory(app)
elseif(ENABLE_TESTS)
    add_subdirectory(tests)
endif()
