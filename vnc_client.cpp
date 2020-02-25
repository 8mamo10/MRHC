#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "opencv2/opencv.hpp"

#include "d3des.h"
#include "mrhc_common.h"
#include "vnc_client.h"

// for log container
std::string mrhc_log = "";

vnc_client::vnc_client(std::string host, int port, std::string password)
    : host(host), port(port), password(password)
{
    this->sockfd = 0;
    this->version = "";
    this->server_init = {};
}

vnc_client::~vnc_client()
{
    close(this->sockfd);
}

bool vnc_client::connect_to_server()
{
    if ((this->sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        return false;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(this->port);
    addr.sin_addr.s_addr = inet_addr(this->host.c_str());

    if (connect(this->sockfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) < 0) {
        return false;
    }
    return true;
}

bool vnc_client::exchange_protocol_version()
{
    char buf[BUF_SIZE] = {};
    int len = 0;

    len = recv(this->sockfd, buf, sizeof(buf), 0);
    if (len < 0) {
        return false;
    }
    log_debug("recv:" + std::to_string(len));
    log_ldebug(buf, len);

    if (memcmp(buf, RFB_PROTOCOL_VERSION_3_3, sizeof(RFB_PROTOCOL_VERSION_3_3)) == 0) {
        this->version = std::string((const char*)RFB_PROTOCOL_VERSION_3_3);
        log_debug("RFB Version 3.3");
    } else if (memcmp(buf, RFB_PROTOCOL_VERSION_3_7, sizeof(RFB_PROTOCOL_VERSION_3_7)) == 0) {
        this->version = std::string((const char*)RFB_PROTOCOL_VERSION_3_7);
        log_debug("RFB Version 3.7");
    } else if (memcmp(buf, RFB_PROTOCOL_VERSION_3_8, sizeof(RFB_PROTOCOL_VERSION_3_8)) == 0) {
        this->version = std::string((const char*)RFB_PROTOCOL_VERSION_3_8);
        log_debug("RFB Version 3.8");
    } else {
        log_debug("Invalid RFB Version");
        return false;
    }
    // send back the same version string
    len = send(this->sockfd, buf, len, 0);
    if (len < 0) {
        return false;
    }
    log_debug("send:" + std::to_string(len));
    log_ldebug(buf, len);

    return true;
}

bool vnc_client::exchange_security_type()
{
    char buf[BUF_SIZE] = {};
    int len = 0;

    len = recv(this->sockfd, buf, sizeof(buf), 0);
    if (len < 0) {
        return false;
    }
    log_debug("recv:" + std::to_string(len));
    log_xdebug(buf, len);

    // specify VNC Authentication
    char security_type = RFB_SECURITY_TYPE_VNC_AUTH;
    len = send(this->sockfd, &security_type, sizeof(security_type), 0);
    if (len < 0) {
        return false;
    }
    log_debug("send:" + std::to_string(len));
    log_ldebug(std::to_string(security_type), len);
    return true;
}

bool vnc_client::vnc_authentication()
{
    char buf[BUF_SIZE] = {};
    int len = 0;

    len = recv(this->sockfd, buf, sizeof(buf), 0);
    if (len < 0) {
        return false;
    }
    log_debug("recv:" + std::to_string(len));
    log_xdebug(buf, len);

    // DES
    unsigned char challenge[RFB_VNC_AUTH_CHALLENGE_LENGTH];
    memmove(challenge, buf, RFB_VNC_AUTH_CHALLENGE_LENGTH);
    deskey((unsigned char*)this->password.c_str(), EN0);
    for (int j = 0; j < RFB_VNC_AUTH_CHALLENGE_LENGTH; j += 8) {
        des(challenge+j, challenge+j);
    }
    len = send(this->sockfd, challenge, RFB_VNC_AUTH_CHALLENGE_LENGTH, 0);
    if (len < 0) {
        return false;
    }
    log_debug("send:" + std::to_string(len));
    log_xdebug(challenge, len);

    // Security result
    memset(buf, 0, BUF_SIZE);
    len = 0;
    len = recv(this->sockfd, buf, sizeof(buf), 0);
    if (len < 0) {
        return false;
    }
    log_debug("recv:" + std::to_string(len));
    log_xdebug(buf, len);

    int security_result = 0;
    memmove(&security_result, buf, len);
    security_result = ntohl(security_result);
    log_debug("securityResult:" + std::to_string(security_result));
    if (security_result != RFB_AUTH_RESULT_OK) {
        log_debug("VNC Authentication failed");
        return false;
    }
    log_debug("VNC Authentication ok");
    return true;
}

bool vnc_client::exchange_init()
{
    char buf[BUF_SIZE] = {};
    int len = 0;

    len = send(this->sockfd, &RFB_SHARED_FLAG_ON, sizeof(RFB_SHARED_FLAG_ON), 0);
    if (len < 0) {
        return false;
    }
    log_debug("send:" + std::to_string(len));
    log_ldebug(std::to_string(RFB_SHARED_FLAG_ON), len);

    len = recv(this->sockfd, buf, sizeof(buf), 0);
    if (len < 0) {
        return false;
    }
    log_debug("recv:" + std::to_string(len));
    log_xdebug(buf, len);

    server_init_t server_init = {};
    memmove(&server_init, buf, len);
    log_debug("frame_buffer_width:" + std::to_string(ntohs(server_init.frame_buffer_width)));
    log_debug("frame_buffer_height:" + std::to_string(ntohs(server_init.frame_buffer_height)));
    log_ldebug(server_init.name_string, (int)sizeof(server_init.name_string));
    this->server_init = server_init;
    return true;
}

bool vnc_client::frame_buffer_update()
{
    char buf[BUF_SIZE*BUF_SIZE] = {};
    int len = 0;

    // set_pixel_format
    set_pixel_format_t set_pixel_format = {};
    pixel_format_t pixel_format = {};
    pixel_format.bits_per_pixel = 0x20;
    pixel_format.depth = 0x20;
    pixel_format.big_endian_flag = 0x00;
    pixel_format.true_colour_flag = 0x01;
    pixel_format.red_max = htons(0x00ff);
    pixel_format.green_max = htons(0x00ff);
    pixel_format.blue_max = htons(0x00ff);
    pixel_format.red_shift = 0x10;
    pixel_format.green_shift = 0x08;
    pixel_format.blue_shift = 0x00;
    set_pixel_format.pixel_format = pixel_format;
    this->server_init.pixel_format = pixel_format;

    len = send(this->sockfd, &set_pixel_format, sizeof(set_pixel_format), 0);
    if (len < 0) {
        return false;
    }
    log_debug("send:" + std::to_string(len));
    log_xdebug(((char*)&set_pixel_format), len);

    // set_encodings
    set_encodings_t set_encodings = {};
    set_encodings.number_of_encodings = htons(1);
    set_encodings.encoding_type = htonl(RFB_ENCODING_RAW);

    len = send(this->sockfd, &set_encodings, sizeof(set_encodings), 0);
    if (len < 0) {
        return false;
    }
    log_debug("send:" + std::to_string(len));
    log_xdebug(((char*)&set_encodings), len);

    // frame_buffer_update_request
    frame_buffer_update_request_t frame_buffer_update_request = {};
    frame_buffer_update_request.incremental = RFB_INCREMENTAL_OFF;
    frame_buffer_update_request.x_position = htons(0);
    frame_buffer_update_request.y_position = htons(0);
    frame_buffer_update_request.width = this->server_init.frame_buffer_width;
    frame_buffer_update_request.height = this->server_init.frame_buffer_height;

    len = send(this->sockfd, &frame_buffer_update_request, sizeof(frame_buffer_update_request), 0);
    if (len < 0) {
        return false;
    }
    log_debug("send:" + std::to_string(len));
    log_xdebug(((char*)&frame_buffer_update_request), len);

    // frame_buffer_update
    frame_buffer_update_t frame_buffer_update = {};
    len = recv(this->sockfd, buf, sizeof(frame_buffer_update), 0);
    if (len < 0) {
        return false;
    }
    log_debug("recv:" + std::to_string(len));
    log_xdebug(buf, len);

    memmove(&frame_buffer_update, buf, sizeof(frame_buffer_update));
    uint8_t message_type = frame_buffer_update.message_type;
    if (message_type != RFB_MESSAGE_TYPE_FRAME_BUFFER_UPDATE) {
        log_debug("unexpected message_type:" + std::to_string(message_type));
        return false;
    }
    uint16_t number_of_rectangles = ntohs(frame_buffer_update.number_of_rectangles);
    log_debug("number_of_rectangles:" + std::to_string(number_of_rectangles));

    // pixel_data
    pixel_data_t data = {};
    memset(buf, 0, sizeof(buf));
    len = recv(this->sockfd, buf, sizeof(data), 0);
    if (len < 0) {
        return false;
    }
    log_debug("recv:" + std::to_string(len));
    log_xdebug(buf, len);

    memmove(&data, buf, sizeof(data));
    uint16_t x_position = ntohs(data.x_position);
    uint16_t y_position = ntohs(data.y_position);
    uint16_t width = ntohs(data.width);
    uint16_t height = ntohs(data.height);
    log_debug("(x_position,y_position,width,height)=(" + std::to_string(x_position) +
              "," + std::to_string(y_position) +
              "," + std::to_string(width) +
              "," + std::to_string(height) + ")");
    int32_t encoding_type = data.encoding_type;
    if (encoding_type != RFB_ENCODING_RAW) {
        log_debug("unexpected encoding_type:" + std::to_string(encoding_type));
        return false;
    }
    // pixels
    uint8_t bits_per_pixel   = this->server_init.pixel_format.bits_per_pixel;
    // uint8_t depth            = this->server_init.pixel_format.depth;
    // uint8_t big_endian_flag  = this->server_init.pixel_format.big_endian_flag;
    // uint8_t true_colour_flag = this->server_init.pixel_format.true_colour_flag;
    // uint16_t red_max         = ntohs(this->server_init.pixel_format.red_max);
    // uint16_t green_max       = ntohs(this->server_init.pixel_format.green_max);
    // uint16_t blue_max        = ntohs(this->server_init.pixel_format.blue_max);
    // uint8_t red_shift        = this->server_init.pixel_format.red_shift;
    // uint8_t green_shift      = this->server_init.pixel_format.green_shift;
    // uint8_t blue_shift       = this->server_init.pixel_format.blue_shift;

    log_debug("---pixel_format---");
    log_debug("bits_per_pixel:"   + std::to_string(bits_per_pixel));
    // log_debug("depth:"            + std::to_string(depth));
    // log_debug("big_endian_flag:"  + std::to_string(big_endian_flag));
    // log_debug("true_colour_flag:" + std::to_string(true_colour_flag));
    // log_debug("red_max:"          + std::to_string(red_max));
    // log_debug("green_max:"        + std::to_string(green_max));
    // log_debug("blue_max:"         + std::to_string(blue_max));
    // log_debug("red_shift:"        + std::to_string(red_shift));
    // log_debug("green_shift:"      + std::to_string(green_shift));
    // log_debug("blue_shift:"       + std::to_string(blue_shift));
    log_debug("------------------");

    int total_pixel_count = width * height;
    int total_pixel_bytes = total_pixel_count * bits_per_pixel / 8;
    log_debug("total_pixel_count:" + std::to_string(total_pixel_count));
    log_debug("expected total_pixel_bytes:" + std::to_string(total_pixel_bytes));

    int total_recv = 0;
    memset(buf, 0, sizeof(buf));
    while ((len = recv(this->sockfd, buf, sizeof(buf), 0)) > 0) {
        log_debug("recv:" + std::to_string(len));
        total_recv += len;
        for (int i = 0; i < len; i+=2) {
            uint16_t pixel = 0;
            memmove(&pixel, &buf[i], sizeof(pixel));
            pixel = ntohs(pixel);
            this->image_buf.push_back(pixel);
        }
        memset(buf, 0, sizeof(buf));
        if (total_recv >= total_pixel_bytes) {
            log_debug("total_recv exceeds total_bytes:" + std::to_string(total_recv));
            break;
        }
    }
    log_debug("image_buf size:" + std::to_string(this->image_buf.size()));

    if (!this->drawImage()) {
        return false;
    }
    return true;
}

std::vector<uint8_t> vnc_client::get_jpeg_buf()
{
    return this->jpeg_buf;
}

//bool vnc_client::drawImage(const char recv_buf[], const int recv_len)
bool vnc_client::drawImage()
{
    uint16_t width = ntohs(this->server_init.frame_buffer_width);
    uint16_t height = ntohs(this->server_init.frame_buffer_height);
    uint8_t bits_per_pixel   = this->server_init.pixel_format.bits_per_pixel;
    uint8_t depth            = this->server_init.pixel_format.depth;
    uint8_t big_endian_flag  = this->server_init.pixel_format.big_endian_flag;
    uint8_t true_colour_flag = this->server_init.pixel_format.true_colour_flag;
    uint16_t red_max         = ntohs(this->server_init.pixel_format.red_max);
    uint16_t green_max       = ntohs(this->server_init.pixel_format.green_max);
    uint16_t blue_max        = ntohs(this->server_init.pixel_format.blue_max);
    uint8_t red_shift        = this->server_init.pixel_format.red_shift;
    uint8_t green_shift      = this->server_init.pixel_format.green_shift;
    uint8_t blue_shift       = this->server_init.pixel_format.blue_shift;

    log_debug("---pixel_format---");
    log_debug("bits_per_pixel:"   + std::to_string(bits_per_pixel));
    log_debug("depth:"            + std::to_string(depth));
    log_debug("big_endian_flag:"  + std::to_string(big_endian_flag));
    log_debug("true_colour_flag:" + std::to_string(true_colour_flag));
    log_debug("red_max:"          + std::to_string(red_max));
    log_debug("green_max:"        + std::to_string(green_max));
    log_debug("blue_max:"         + std::to_string(blue_max));
    log_debug("red_shift:"        + std::to_string(red_shift));
    log_debug("green_shift:"      + std::to_string(green_shift));
    log_debug("blue_shift:"       + std::to_string(blue_shift));
    log_debug("------------------");

    // sample drawing
    // cv::Mat image = cv::Mat(height, width, CV_8UC3, cv::Scalar(0, 0, 0));
    // rectangle(image, cv::Point(10, 10), cv::Point(100, 100), cv::Scalar(128, 0, 0), -1, cv::LINE_AA);
    // rectangle(image, cv::Point(200, 200), cv::Point(300, 300), cv::Scalar(0, 128, 0), -1, cv::LINE_AA);
    // rectangle(image, cv::Point(500, 500), cv::Point(700, 700), cv::Scalar(0, 0, 128), -1, cv::LINE_AA);
    // cv::imencode(".jpeg", image, this->image_buf);

    cv::Mat image = cv::Mat(height, width, CV_8UC3, cv::Scalar(0, 0, 0));
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            uint16_t pixel = this->image_buf[width * y + x];
            uint8_t red = ((pixel >> red_shift) & red_max);
            uint8_t green = ((pixel >> green_shift) & green_max);
            uint8_t blue = ((pixel >> blue_shift) & blue_max);
            // log_debug("(R,G,B)=(" + std::to_string(red) +
            //           "," + std::to_string(green) +
            //           "," + std::to_string(blue) + ")");
            rectangle(image, cv::Point(x, y), cv::Point(x+1, y+1), cv::Scalar(blue, green, red), -1, cv::LINE_AA);
        }
    }
    cv::imencode(".jpeg", image, this->jpeg_buf);
    return true;
}
