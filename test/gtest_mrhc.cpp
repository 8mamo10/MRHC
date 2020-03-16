#include "gtest/gtest.h"
#include "mrhc_common.h"
#include "vnc_client.h"

namespace {

    class mrhc_test : public ::testing::Test {};
    // g++ mrhc_test.cpp -std=c++11 -lgtest -lgtest_main -lpthread -I/usr/local/include/gtest/
    // -I../ -I/usr/local/apr/include  -I/usr/local/apr/include/apr-1/ -I/usr/local/apache2/include
    // ../vnc_client.o ../logger.o ../d3des.o `pkg-config --libs opencv4`
    TEST_F(mrhc_test, test_get_width)
    {
        vnc_client v = vnc_client("", 0, "");
        EXPECT_EQ(0, v.get_width());
    }

};
