#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "vnc_client.h"

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
    char buf[VncClient::BUF_SIZE] = {};
    int len = 0;

    len = recv(this->sockfd, buf, sizeof(buf), 0);
    if (len < 0) {
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
    char buf[VncClient::BUF_SIZE] = {};
    int len = 0;

    len = recv(this->sockfd, buf, sizeof(buf), 0);
    if (len < 0) {
        return false;
    }
    // specify VNC Authentication
    char securityType = VncClient::SECURITY_TYPE_VNC_AUTH;
    len = send(this->sockfd, &securityType, sizeof(securityType), 0);
    if (len < 0) {
        return false;
    }
    return true;
}
