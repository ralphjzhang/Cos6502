project(cp6502)

include_directories(${CMAKE_SOURCE_DIR})

add_executable(test_cp6502
    utest/test_LoadRegister.cpp
    utest/test_StoreRegister.cpp
    utest/test_JumpsAndCalls.cpp
    utest/test_StackOperations.cpp
    utest/test_Logical.cpp
    utest/test_LoadProgram.cpp
    core/cp6502.cpp
    )
target_link_libraries(test_cp6502 gtest_main gtest pthread)

add_test(NAME cp6502_test COMMAND cp6502_test)
