#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "vnc_client.h"

VncClient::VncClient(std::string host, int port, std::string password)
    : host(host), port(port), password(password)
{
}

bool VncClient::connectToServer()
{
    int sockfd;
    struct sockaddr_in addr;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        return false;
    }
    addr.sin_family = AF_INET;
    addr.sin_port = htons(this->port);
    addr.sin_addr.s_addr = inet_addr(this->host.c_str());

    if (connect(sockfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) < 0) {
        return false;
    }
    return true;
}
