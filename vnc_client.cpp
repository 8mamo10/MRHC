#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "mrhc_common.h"
#include "vnc_client.h"

const string VncClient::PROTOCOL_VERSION_3_3 = "RFB 003.003\n";
const string VncClient::PROTOCOL_VERSION_3_7 = "RFB 003.007\n";
const string VncClient::PROTOCOL_VERSION_3_8 = "RFB 003.008\n";
const char VncClient::SECURITY_TYPE_VNC_AUTH = 0x02;

VncClient::VncClient(string host, int port, string password)
    : host(host), port(port), password(password)
{
    this->sockfd = 0;
}

bool VncClient::connectToServer()
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

bool VncClient::exchangeProtocolVersion()
{
    char buf[BUF_SIZE] = {};
    int len = 0;

    len = recv(this->sockfd, buf, sizeof(buf), 0);
    if (len < 0) {
        return false;
    }
    log_debug("recv:" + to_string(len));
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
    return true;
}

bool VncClient::exchangeSecurityType()
{
    char buf[BUF_SIZE] = {};
    int len = 0;

    len = recv(this->sockfd, buf, sizeof(buf), 0);
    if (len < 0) {
        return false;
    }
    log_debug("recv:" + to_string(len));
    log_xdebug(buf, len);

    // specify VNC Authentication
    char securityType = SECURITY_TYPE_VNC_AUTH;
    len = send(this->sockfd, &securityType, sizeof(securityType), 0);
    if (len < 0) {
        return false;
    }
    return true;
}
