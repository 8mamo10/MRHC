#ifndef __VNC_CLIENT_H__
#define __VNC_CLIENT_H__

#include <bits/stdc++.h>

class VncClient
{
 private:
    std::string host;
    std::string password;

    std::string parseHost();
    int parsePort();
 public:
    VncClient();
    void setConnectionInfo(std::string host, std::string password);
    bool connect();
};

#endif
