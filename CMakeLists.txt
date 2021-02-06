project(Cos6502)

include_directories(${CMAKE_SOURCE_DIR})

add_executable(cos6502_test test/cos6502.cpp)
target_link_libraries(cos6502_test gtest_main gtest pthread)

add_test(NAME cos6502_test COMMAND cos6502_test)
