#ifndef __VNC_CLIENT_H__
#define __VNC_CLIENT_H__

#include "rfb_protocol.h"

class vnc_client
{
 private:
    int sockfd;

    // for connection
    std::string host;
    int port;
    std::string password;

    // handshaking
    std::string version;
    std::vector<uint8_t> security_types;
    uint8_t challenge[RFB_VNC_AUTH_CHALLENGE_LENGTH];
    // initialisation
    uint16_t width = 0;
    uint16_t height = 0;
    pixel_format_t pixel_format;
    std::string name;
    // for output
    std::vector<uint32_t> image_buf;
    std::vector<uint8_t> jpeg_buf;

    bool recv_rectangles(uint16_t number_of_rectangles);
    bool recv_rectangle();
 public:
    vnc_client(std::string host, int port, std::string password);
    ~vnc_client();
    bool connect_to_server();

    bool recv_protocol_version();
    bool send_protocol_version();
    bool recv_supported_security_types();
    bool send_security_type();
    bool recv_vnc_auth_challenge();
    bool send_vnc_auth_response();
    bool recv_security_result();
    bool send_client_init();
    bool recv_server_init();
    bool send_set_pixel_format();
    bool send_set_encodings();
    bool send_frame_buffer_update_request();
    bool recv_frame_buffer_update();
    bool draw_image();
    void clear_buf();

    std::vector<uint8_t> get_jpeg_buf();
    uint16_t get_width();
    uint16_t get_height();
};

#endif
