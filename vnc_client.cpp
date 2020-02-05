#include "vnc_client.h"

VncClient::VncClient()
{
}

void VncClient::setConnectionInfo(std::string host, std::string password)
{
    this->host = host;
    this->password = password;
}
bool VncClient::connect()
{
    return true;
}
