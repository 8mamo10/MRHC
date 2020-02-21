#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "d3des.h"
#include "mrhc_common.h"
#include "vnc_client.h"

const std::string vnc_client::PROTOCOL_VERSION_3_3 = "RFB 003.003\n";
const std::string vnc_client::PROTOCOL_VERSION_3_7 = "RFB 003.007\n";
const std::string vnc_client::PROTOCOL_VERSION_3_8 = "RFB 003.008\n";
const char vnc_client::SECURITY_TYPE_VNC_AUTH = 0x02;
const int vnc_client::VNC_AUTH_PASSWORD_LENGTH = 16;
const int vnc_client::VNC_AUTH_RESULT_OK = 0x00;
const int vnc_client::VNC_AUTH_RESULT_FAILED = 0x01;
const char vnc_client::VNC_SHARED_FLAG_ON = 0x01;
const char vnc_client::VNC_SHARED_FLAG_OFF = 0x00;

// for log container
std::string mrhc_log = "";

vnc_client::vnc_client(std::string host, int port, std::string password)
    : host(host), port(port), password(password)
{
    this->sockfd = 0;
}

vnc_client::~vnc_client()
{
    close(this->sockfd);
}

bool vnc_client::connect_to_server()
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

bool vnc_client::exchange_protocol_version()
{
    char buf[BUF_SIZE] = {};
    int len = 0;

    len = recv(this->sockfd, buf, sizeof(buf), 0);
    if (len < 0) {
        return false;
    }
    log_debug("recv:" + std::to_string(len));
    log_ldebug(buf, len);

    if (buf == PROTOCOL_VERSION_3_3) {
        version = PROTOCOL_VERSION_3_3;
        log_debug("RFB Version 3.3");
    } else if (buf == PROTOCOL_VERSION_3_7) {
        version = PROTOCOL_VERSION_3_7;
        log_debug("RFB Version 3.7");
    } else if (buf == PROTOCOL_VERSION_3_8) {
        version = PROTOCOL_VERSION_3_8;
        log_debug("RFB Version 3.8");
    } else {
        log_debug("Invalid RFB Version");
        return false;
    }
    // send back the same version string
    len = send(this->sockfd, buf, len, 0);
    if (len < 0) {
        return false;
    }
    log_debug("send:" + std::to_string(len));
    log_ldebug(buf, len);

    return true;
}

bool vnc_client::exchange_security_type()
{
    char buf[BUF_SIZE] = {};
    int len = 0;

    len = recv(this->sockfd, buf, sizeof(buf), 0);
    if (len < 0) {
        return false;
    }
    log_debug("recv:" + std::to_string(len));
    log_xdebug(buf, len);

    // specify VNC Authentication
    char security_type = SECURITY_TYPE_VNC_AUTH;
    len = send(this->sockfd, &security_type, sizeof(security_type), 0);
    if (len < 0) {
        return false;
    }
    log_debug("send:" + std::to_string(len));
    log_ldebug(std::to_string(security_type), len);
    return true;
}

bool vnc_client::vnc_authentication()
{
    char buf[BUF_SIZE] = {};
    int len = 0;

    len = recv(this->sockfd, buf, sizeof(buf), 0);
    if (len < 0) {
        return false;
    }
    log_debug("recv:" + std::to_string(len));
    log_xdebug(buf, len);

    // DES
    unsigned char challenge[VNC_AUTH_PASSWORD_LENGTH];
    memmove(challenge, buf, VNC_AUTH_PASSWORD_LENGTH);
    deskey((unsigned char*)this->password.c_str(), EN0);
    for (int j = 0; j < VNC_AUTH_PASSWORD_LENGTH; j += 8) {
        des(challenge+j, challenge+j);
    }
    len = send(this->sockfd, challenge, VNC_AUTH_PASSWORD_LENGTH, 0);
    if (len < 0) {
        return false;
    }
    log_debug("send:" + std::to_string(len));
    log_xdebug(challenge, len);

    // Security result
    memset(buf, 0, BUF_SIZE);
    len = 0;
    len = recv(this->sockfd, buf, sizeof(buf), 0);
    if (len < 0) {
        return false;
    }
    log_debug("recv:" + std::to_string(len));
    log_xdebug(buf, len);

    int security_result = 0;
    memmove(&security_result, buf, len);
    security_result = ntohl(security_result);
    log_debug("securityResult:" + std::to_string(security_result));
    if (security_result != VNC_AUTH_RESULT_OK) {
        log_debug("VNC Authentication failed");
        return false;
    }
    log_debug("VNC Authentication ok");
    return true;
}

bool vnc_client::exchange_init()
{
    char buf[BUF_SIZE] = {};
    int len = 0;

    len = send(this->sockfd, &VNC_SHARED_FLAG_ON, sizeof(VNC_SHARED_FLAG_ON), 0);
    if (len < 0) {
        return false;
    }
    log_debug("send:" + std::to_string(len));
    log_ldebug(std::to_string(VNC_SHARED_FLAG_ON), len);

    len = recv(this->sockfd, buf, sizeof(buf), 0);
    if (len < 0) {
        return false;
    }
    log_debug("recv:" + std::to_string(len));
    log_xdebug(buf, len);

    server_init_t server_init = {};
    memmove(&server_init, buf, len);
    log_debug("frame_buffer_width:" + std::to_string(ntohs(server_init.frame_buffer_width)));
    log_debug("frame_buffer_height:" + std::to_string(ntohs(server_init.frame_buffer_height)));
    log_ldebug(server_init.name_string, (int)sizeof(server_init.name_string));
    return true;
}
