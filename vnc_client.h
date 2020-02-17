#ifndef __VNC_CLIENT_H__
#define __VNC_CLIENT_H__

class VncClient
{
 private:

    static const string PROTOCOL_VERSION_3_3;
    static const string PROTOCOL_VERSION_3_7;
    static const string PROTOCOL_VERSION_3_8;
    static const char SECURITY_TYPE_VNC_AUTH;

    int sockfd;

    string host;
    int port;
    string password;

    string version;

 public:
    VncClient(string host, int port, string password);
    bool connectToServer();
    bool exchangeProtocolVersion();
    bool exchangeSecurityType();
};

#endif
