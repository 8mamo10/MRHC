#ifndef __VNC_CLIENT_H__
#define __VNC_CLIENT_H__

#include "rfb_protocol.h"

class vnc_client
{
 private:

    static const std::string PROTOCOL_VERSION_3_3;
    static const std::string PROTOCOL_VERSION_3_7;
    static const std::string PROTOCOL_VERSION_3_8;
    static const char SECURITY_TYPE_VNC_AUTH;
    static const int VNC_AUTH_PASSWORD_LENGTH;
    static const int VNC_AUTH_RESULT_OK;
    static const int VNC_AUTH_RESULT_FAILED;
    static const char VNC_SHARED_FLAG_ON;
    static const char VNC_SHARED_FLAG_OFF;

    int sockfd;

    std::string host;
    int port;
    std::string password;

    std::string version;

 public:
    vnc_client(std::string host, int port, std::string password);
    ~vnc_client();
    bool connect_to_server();
    bool exchange_protocol_version();
    bool exchange_security_type();
    bool vnc_authentication();
    bool exchange_init();
};

#endif
