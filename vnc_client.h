#ifndef __VNC_CLIENT_H__
#define __VNC_CLIENT_H__


#include "opencv2/core/core.hpp"

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
    // initialization
    uint16_t width = 0;
    uint16_t height = 0;
    pixel_format_t pixel_format;
    std::string name;
    // output
    cv::Mat image;
    std::vector<uint32_t> image_buf;
    std::vector<uint8_t> jpeg_buf;

    bool recv_rectangles(uint16_t number_of_rectangles);
    bool recv_rectangle();
 public:
    vnc_client(std::string host, int port, std::string password);
    ~vnc_client();
    // interface to drive vnc client by mod_mrhc
    bool initialize();
    bool authenticate();
    bool configure();
    bool operate(uint16_t x, uint16_t y, uint8_t button);
    bool capture();
    // getter
    const std::vector<uint8_t> get_jpeg_buf() const { return this->jpeg_buf; };
    const uint16_t get_width() const { return this->width; };
    const uint16_t get_height() const { return this->height; };
    const std::string get_version() const { return this->version; }
    ////// make the following public for testing //////
    bool connect_to_server();
    // RFB Protocol (see also: rfb_protocol.h)
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
    bool send_pointer_event(uint16_t x, uint16_t y, uint8_t button);
    bool draw_image();
    bool draw_pointer(uint16_t x, uint16_t y);
    void clear_buf();
};

#endif
