project(cp6502)

set(CMAKE_CXX_STANDARD 20)

include_directories(${CMAKE_SOURCE_DIR})

add_executable(test_cp6502
    utest/test_LoadRegister.cpp
    utest/test_StoreRegister.cpp
    utest/test_JumpsAndCalls.cpp
    utest/test_StackOperations.cpp
    utest/test_Logical.cpp
    utest/test_LoadProgram.cpp
    utest/test_RegisterTransfers.cpp
    utest/test_IncrementDecrements.cpp
    utest/test_Branches.cpp
    utest/test_StatusFlagChanges.cpp
    utest/test_AddWithCarry.cpp
    utest/test_Compare.cpp
    utest/test_SubstractWithCarry.cpp
    utest/test_Shifts.cpp
    core/cp6502.cpp
    )
target_link_libraries(test_cp6502 gtest_main gtest pthread)

add_test(NAME cp6502_test COMMAND cp6502_test)
