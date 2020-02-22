#ifndef __VNC_CLIENT_H__
#define __VNC_CLIENT_H__

#include "rfb_protocol.h"

class vnc_client
{
 private:

    std::string host;
    int port;
    std::string password;

    int sockfd;

    std::string version;
    server_init_t server_init;

 public:
    vnc_client(std::string host, int port, std::string password);
    ~vnc_client();
    bool connect_to_server();
    bool exchange_protocol_version();
    bool exchange_security_type();
    bool vnc_authentication();
    bool exchange_init();
    bool frame_buffer_update();
};

#endif
