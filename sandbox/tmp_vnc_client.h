#include <iostream>
#include <string>

#define BUF_SIZE 1024

class vnc_client
{
public:
    int sockfd;
    std::string host;
    int port;
    std::string password;

    vnc_client() {
        this->sockfd = 0;
        this->host = "127.0.0.1";
        this->port = 6623;
        this->password = "testtest";
    };
    bool connect_to_server()
    {
        if ((this->sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            return false;
        }
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(this->port);
        addr.sin_addr.s_addr = inet_addr(this->host.c_str());
        if (connect(this->sockfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) < 0) {
            return false;
        }
        return true;
    }
    bool recv_protocol_version() {
        char buf[BUF_SIZE] = {};
        int length = recv(this->sockfd, buf, sizeof(buf), 0);
        if (length < 0) {
            return false;
        }
        std::cout << "recv: " << length << std::endl;
        return true;
    }
    bool send_protocol_version()
    {
        // RFB 003.003\n
        const uint8_t RFB_PROTOCOL_VERSION_3_3[] = {0x52, 0x46, 0x42, 0x20, 0x30, 0x30, 0x33, 0x2e, 0x30, 0x30, 0x33, 0x0a};
        int length = send(this->sockfd, &RFB_PROTOCOL_VERSION_3_3, 12, 0);
        if (length < 0) {
            return false;
        }
        std::cout << "send: " << length << std::endl;
        return true;
    }
    std::string get_host() { return this->host; }
};
