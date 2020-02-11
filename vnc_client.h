#ifndef __VNC_CLIENT_H__
#define __VNC_CLIENT_H__

#include <bits/stdc++.h>

using namespace std;

class VncClient
{
 private:

    static const int BUF_SIZE = 1024;

    int sockfd;

    string host;
    int port;
    string password;

 public:
    VncClient(string host, int port, string password);
    bool connectToServer();
    bool exchangeProtocolVersion();
};

#endif
