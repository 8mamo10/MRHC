#include "vnc_client.h"

VncClient::VncClient(std::string host, int port, std::string password)
    : host(host), port(port), password(password)
{
}

bool VncClient::connect()
{
    return true;
}
