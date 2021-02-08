project(Cos6502)

include_directories(${CMAKE_SOURCE_DIR})

add_executable(test_cos6502
    test/test_loadregister.cpp
    test/test_storeregister.cpp
    core/cos6502.cpp
    )
target_link_libraries(test_cos6502 gtest_main gtest pthread)

add_test(NAME cos6502_test COMMAND cos6502_test)
