add_executable(sgdcore_test
    test_core.cpp
    test_ops.cpp
    test_autodiff.cpp
    test_verification.cpp
)

target_link_libraries(sgdcore_test PRIVATE sgdcore GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(sgdcore_test)