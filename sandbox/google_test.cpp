#include "gtest/gtest.h"
#include "mrhc_common.h"
#include "vnc_client.h"

namespace {

    class MyFirstTest : public ::testing::Test {};
    // g++ google_test.cpp -std=c++11 -I/usr/local/include/gtest/ -lgtest -lgtest_main -pthread
    TEST_F(MyFirstTest, Test1)
    {
        int a = 1;
        EXPECT_EQ(1, a);
    }
    // g++ google_test.cpp -std=c++11 -lgtest -lgtest_main -lpthread -I/usr/local/include/gtest/
    // -I../ -I/usr/local/apr/include  -I/usr/local/apr/include/apr-1/ -I/usr/local/apache2/include
    // ../vnc_client.o ../logger.o ../d3des.o `pkg-config --libs opencv4`
    TEST_F(MyFirstTest, Test2)
    {
        vnc_client v = vnc_client("", 0, "");
        EXPECT_EQ(0, v.get_width());
    }

};
