#include "gtest/gtest.h"

#include "info.h"

int main(int argc, char** argv)
{
    print_info();

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}