#include "gtest/gtest.h"
#include "mrhc_common.h"
#include "vnc_client.h"

namespace {

    class mrhc_test : public ::testing::Test {

    // protected:
    //     virtual void SetUp() {
    //         this->v = new vnc_client("127.0.0.1", 6624, "testtest");
    //     }
    //     virtual void TearDown() {
    //         delete this->v;
    //     }
    //     vnc_client *v;
    };

    // g++ mrhc_test.cpp -std=c++11 -lgtest -lgtest_main -lpthread -I/usr/local/include/gtest/
    // -I../ -I/usr/local/apr/include  -I/usr/local/apr/include/apr-1/ -I/usr/local/apache2/include
    // ../vnc_client.o ../logger.o ../d3des.o `pkg-config --libs opencv4`

    TEST_F(mrhc_test, test_get_width)
    {
        vnc_client v = vnc_client("", 0, "");
        EXPECT_EQ(0, v.get_width());
    }

    TEST_F(mrhc_test, test_get_height)
    {
        vnc_client v = vnc_client("", 0, "");
        EXPECT_EQ(0, v.get_height());
    }

    TEST_F(mrhc_test, test_connect_to_server)
    {
        vnc_client v = vnc_client("127.0.0.1", 6624, "testtest");
        bool ret = v.connect_to_server();
        EXPECT_EQ(true, ret);
    }

    TEST_F(mrhc_test, test_recv_protocol_version_3_3)
    {
        vnc_client v = vnc_client("127.0.0.1", 6624, "testtest");
        bool ret = v.connect_to_server();
        ret = v.recv_protocol_version();
        EXPECT_EQ(true, ret);
        std::string version = std::string({0x52, 0x46, 0x42, 0x20, 0x30, 0x30, 0x33, 0x2e, 0x30, 0x30, 0x33, 0x0a});
        EXPECT_NE(version, v.get_version());
    }

    TEST_F(mrhc_test, test_recv_protocol_version_3_8)
    {
        vnc_client v = vnc_client("127.0.0.1", 6624, "testtest");
        bool ret = v.connect_to_server();
        ret = v.recv_protocol_version();
        EXPECT_EQ(true, ret);
        std::string version = std::string({0x52, 0x46, 0x42, 0x20, 0x30, 0x30, 0x33, 0x2e, 0x30, 0x30, 0x38, 0x0a});
        EXPECT_EQ(version, v.get_version());
    }

    TEST_F(mrhc_test, test_vnc_auth)
    {
        vnc_client v = vnc_client("127.0.0.1", 6624, "testtest");
        bool ret = v.connect_to_server();
        ret = v.recv_protocol_version();
        EXPECT_EQ(true, ret);
        // RFB 003.008
        std::string version = std::string({0x52, 0x46, 0x42, 0x20, 0x30, 0x30, 0x33, 0x2e, 0x30, 0x30, 0x38, 0x0a});
        EXPECT_EQ(version, v.get_version());
        ret = v.send_protocol_version();
        EXPECT_EQ(true, ret);
    }

};
