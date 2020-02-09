#ifndef __VNC_CLIENT_H__
#define __VNC_CLIENT_H__

#include <bits/stdc++.h>

class VncClient
{
 private:
    std::string host;
    int port;
    std::string password;

 public:
    VncClient(std::string host, int port, std::string password);
    bool connectToServer();
};

#endif
