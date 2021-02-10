project(cp6502)

include_directories(${CMAKE_SOURCE_DIR})

add_executable(test_cp6502
    test/test_LoadRegister.cpp
    test/test_StoreRegister.cpp
    test/test_JumpsAndCalls.cpp
    test/test_StackOperations.cpp
    test/test_Logical.cpp
    core/cp6502.cpp
    )
target_link_libraries(test_cp6502 gtest_main gtest pthread)

add_test(NAME cp6502_test COMMAND cp6502_test)
