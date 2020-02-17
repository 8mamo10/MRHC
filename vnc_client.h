#ifndef __VNC_CLIENT_H__
#define __VNC_CLIENT_H__

class VncClient
{
 private:

    static const string PROTOCOL_VERSION_3_3;
    static const string PROTOCOL_VERSION_3_7;
    static const string PROTOCOL_VERSION_3_8;
    static const char SECURITY_TYPE_VNC_AUTH;
    static const int VNC_AUTH_PASSWORD_LENGTH;
    static const int VNC_AUTH_RESULT_OK;
    static const int VNC_AUTH_RESULT_FAILED;

    int sockfd;

    string host;
    int port;
    string password;

    string version;

 public:
    VncClient(string host, int port, string password);
    ~VncClient();
    bool connectToServer();
    bool exchangeProtocolVersion();
    bool exchangeSecurityType();
    bool vncAuthentication();
};

#endif
