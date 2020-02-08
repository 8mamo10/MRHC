#ifndef __VNC_CLIENT_H__
#define __VNC_CLIENT_H__

#include <bits/stdc++.h>

class VncClient
{
 private:
    std::string host;
    std::string password;

 public:
    VncClient();
    void setConnectionInfo(std::string host, std::string password);
    bool connect();
};

#endif
