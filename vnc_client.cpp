#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "opencv2/opencv.hpp"

#include "d3des.h"
#include "mrhc_common.h"
#include "vnc_client.h"

// for log container
std::string mrhc_log = "";

//// public /////

vnc_client::vnc_client(std::string host, int port, std::string password)
    : sockfd(0), host(host), port(port), password(password), version(""),  width(0), height(0), pixel_format({}), name("")
{
    memset(this->challenge, 0, sizeof(this->challenge));
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

bool vnc_client::recv_protocol_version()
{
    char buf[BUF_SIZE] = {};
    int length = recv(this->sockfd, buf, sizeof(buf), 0);
    if (length < 0) {
        return false;
    }
    log_debug("recv:" + std::to_string(length));
    log_ldebug(buf, length);

    protocol_version_t protocol_version = {};
    memmove(&protocol_version, buf, length);

    if (memcmp(protocol_version.values, RFB_PROTOCOL_VERSION_3_3, sizeof(RFB_PROTOCOL_VERSION_3_3)) == 0) {
        this->version = std::string((const char*)RFB_PROTOCOL_VERSION_3_3);
        log_debug("RFB Version 3.3");
    } else if (memcmp(protocol_version.values, RFB_PROTOCOL_VERSION_3_7, sizeof(RFB_PROTOCOL_VERSION_3_7)) == 0) {
        this->version = std::string((const char*)RFB_PROTOCOL_VERSION_3_7);
        log_debug("RFB Version 3.7");
    } else if (memcmp(protocol_version.values, RFB_PROTOCOL_VERSION_3_8, sizeof(RFB_PROTOCOL_VERSION_3_8)) == 0) {
        this->version = std::string((const char*)RFB_PROTOCOL_VERSION_3_8);
        log_debug("RFB Version 3.8");
    } else {
        log_debug("Invalid RFB Version");
        return false;
    }
    return true;
}

bool vnc_client::send_protocol_version()
{
    protocol_version_t protocol_version = {};
    memmove(protocol_version.values, this->version.c_str(), this->version.length());
    // send back the same version string
    int length = send(this->sockfd, &protocol_version, sizeof(protocol_version), 0);
    if (length < 0) {
        return false;
    }
    log_debug("send:" + std::to_string(length));
    log_ldebug(((char*)&protocol_version), length);
    return true;
}

bool vnc_client::recv_supported_security_types()
{
    char buf[BUF_SIZE] = {};
    int length = recv(this->sockfd, buf, sizeof(buf), 0);
    if (length < 0) {
        return false;
    }
    log_debug("recv:" + std::to_string(length));
    log_xdebug(buf, length);

    supported_security_types_t supported_security_types = {};
    memmove(&supported_security_types, buf, length);

    uint8_t num = supported_security_types.number_of_security_types;
    for (int i = 0; i < num; i++) {
        this->security_types.push_back(supported_security_types.security_types[i]);
    }
    return true;
}

bool vnc_client::send_security_type()
{
    security_type_t security_type = {};
    // specify VNC Authentication
    security_type.value = RFB_SECURITY_TYPE_VNC_AUTH;
    int length = send(this->sockfd, &security_type, sizeof(security_type), 0);
    if (length < 0) {
        return false;
    }
    log_debug("send:" + std::to_string(length));
    log_xdebug(((char*)&security_type), length);
    return true;
}

bool vnc_client::recv_vnc_auth_challenge()
{
    char buf[BUF_SIZE] = {};
    int length = recv(this->sockfd, buf, sizeof(buf), 0);
    if (length < 0) {
        return false;
    }
    log_debug("recv:" + std::to_string(length));
    log_xdebug(buf, length);

    memmove(this->challenge, buf, length);
    return true;
}

bool vnc_client::send_vnc_auth_response()
{
    // DES
    deskey((unsigned char*)this->password.c_str(), EN0);
    for (int j = 0; j < RFB_VNC_AUTH_CHALLENGE_LENGTH; j += 8) {
        des(this->challenge+j, this->challenge+j);
    }
    int length = send(this->sockfd, this->challenge, RFB_VNC_AUTH_CHALLENGE_LENGTH, 0);
    if (length < 0) {
        return false;
    }
    log_debug("send:" + std::to_string(length));
    log_xdebug(this->challenge, length);
    return true;
}

bool vnc_client::recv_security_result()
{
    char buf[BUF_SIZE] = {};
    int length = recv(this->sockfd, buf, sizeof(buf), 0);
    if (length < 0) {
        return false;
    }
    log_debug("recv:" + std::to_string(length));
    log_xdebug(buf, length);

    security_result_t security_result = {};
    memmove(&security_result, buf, length);
    uint32_t status = ntohl(security_result.status);
    log_debug("status:" + std::to_string(status));
    if (status != RFB_SECURITY_RESULT_OK) {
        log_debug("VNC Authentication failed");
        return false;
    }
    log_debug("VNC Authentication ok");
    return true;
}

bool vnc_client::send_client_init()
{
    client_init_t client_init = {};
    client_init.shared_flag = RFB_SHARED_FLAG_ON;

    int length = send(this->sockfd, &client_init, sizeof(client_init), 0);
    if (length < 0) {
        return false;
    }
    log_debug("send:" + std::to_string(length));
    log_xdebug(((char*)&client_init), length);
    return true;
}

bool vnc_client::recv_server_init()
{
    char buf[sizeof(server_init_t)] = {};
    int length = recv(this->sockfd, buf, sizeof(buf), 0);
    if (length < 0) {
        return false;
    }
    log_debug("recv:" + std::to_string(length));
    log_xdebug(buf, length);

    server_init_t server_init = {};
    memmove(&server_init, buf, length);

    this->width = ntohs(server_init.frame_buffer_width);
    this->height = ntohs(server_init.frame_buffer_height);
    char name[BUF_SIZE] = {};
    memmove(name, server_init.name_string, ntohl(server_init.name_length));
    this->name = std::string(name);

    log_debug("frame_buffer_width:" + std::to_string(this->width));
    log_debug("frame_buffer_height:" + std::to_string(this->height));
    log_debug("name:" + this->name);

    return true;
}

bool vnc_client::send_set_pixel_format()
{
    set_pixel_format_t set_pixel_format = {};
    pixel_format_t pixel_format = {};
    pixel_format.bits_per_pixel = 0x20;
    pixel_format.depth = 0x20;
    pixel_format.big_endian_flag = 0x00;
    pixel_format.true_colour_flag = 0x01;
    pixel_format.red_max = htons(0xff);
    pixel_format.green_max = htons(0xff);
    pixel_format.blue_max = htons(0xff);
    pixel_format.red_shift = 0x10;
    pixel_format.green_shift = 0x08;
    pixel_format.blue_shift = 0x00;
    set_pixel_format.pixel_format = pixel_format;
    this->pixel_format = pixel_format;

    int length = send(this->sockfd, &set_pixel_format, sizeof(set_pixel_format), 0);
    if (length < 0) {
        return false;
    }
    log_debug("send:" + std::to_string(length));
    log_xdebug(((char*)&set_pixel_format), length);
    return true;
}

bool vnc_client::send_set_encodings()
{
    set_encodings_t set_encodings = {};
    set_encodings.number_of_encodings = htons(1);
    set_encodings.encoding_type = htonl(RFB_ENCODING_RAW);

    int length = send(this->sockfd, &set_encodings, sizeof(set_encodings), 0);
    if (length < 0) {
        return false;
    }
    log_debug("send:" + std::to_string(length));
    log_xdebug(((char*)&set_encodings), length);
    return true;
}

bool vnc_client::send_frame_buffer_update_request()
{
    frame_buffer_update_request_t frame_buffer_update_request = {};
    frame_buffer_update_request.incremental = RFB_INCREMENTAL_OFF;
    frame_buffer_update_request.x_position = htons(0);
    frame_buffer_update_request.y_position = htons(0);
    frame_buffer_update_request.width = htons(this->width);
    frame_buffer_update_request.height = htons(this->height);

    int length = send(this->sockfd, &frame_buffer_update_request, sizeof(frame_buffer_update_request), 0);
    if (length < 0) {
        return false;
    }
    log_debug("send:" + std::to_string(length));
    log_xdebug(((char*)&frame_buffer_update_request), length);
    return true;
}

bool vnc_client::recv_frame_buffer_update()
{
    char buf[BUF_SIZE] = {};

    frame_buffer_update_t frame_buffer_update = {};
    int length = recv(this->sockfd, buf, sizeof(frame_buffer_update), 0);
    if (length < 0) {
        return false;
    }
    log_debug("recv:" + std::to_string(length));
    log_xdebug(buf, length);

    memmove(&frame_buffer_update, buf, length);
    uint8_t message_type = frame_buffer_update.message_type;
    if (message_type != RFB_MESSAGE_TYPE_FRAME_BUFFER_UPDATE) {
        log_debug("unexpected message_type:" + std::to_string(message_type));
        return false;
    }
    uint16_t number_of_rectangles = ntohs(frame_buffer_update.number_of_rectangles);
    log_debug("number_of_rectangles:" + std::to_string(number_of_rectangles));

    this->recv_rectangles(number_of_rectangles);
    return true;
}

bool vnc_client::send_pointer_event(uint16_t x_position, uint16_t y_position)
{
    uint8_t button_mask = 0;
    // only left button
    button_mask |= (RFB_POINTER_DOWN << 0);

    pointer_event_t pointer_event = {};
    pointer_event.button_mask = button_mask;
    pointer_event.x_position = htons(x_position);
    pointer_event.y_position = htons(y_position);

    // send down
    int length = send(this->sockfd, &pointer_event, sizeof(pointer_event), 0);
    if (length < 0) {
        return false;
    }
    log_debug("send:" + std::to_string(length));
    log_xdebug(((char*)&pointer_event), length);

    // send up
    button_mask = 0;
    pointer_event.button_mask = button_mask;
    length = send(this->sockfd, &pointer_event, sizeof(pointer_event), 0);
    if (length < 0) {
        return false;
    }
    log_debug("send:" + std::to_string(length));
    log_xdebug(((char*)&pointer_event), length);
    return true;
}

//// private /////

bool vnc_client::recv_rectangles(uint16_t number_of_rectangles)
{
    for (int i = 0; i < number_of_rectangles; i++) {
        log_debug("--start recv_rectangle:" + std::to_string(i + 1));
        if (!this->recv_rectangle()) {
            log_debug("failed to recv_rectangle");
            return false;
        }
        log_debug("--finish recv_rectangle:" + std::to_string(i + 1));
    }
    return true;
}

bool vnc_client::recv_rectangle()
{
    char buf[BUF_SIZE] = {};

    pixel_data_t pixel_data = {};
    int length = recv(this->sockfd, buf, sizeof(pixel_data), 0);
    if (length < 0) {
        return false;
    }
    log_debug("recv:" + std::to_string(length));
    log_xdebug(buf, length);

    memmove(&pixel_data, buf, length);
    uint16_t x_position = ntohs(pixel_data.x_position);
    uint16_t y_position = ntohs(pixel_data.y_position);
    uint16_t width = ntohs(pixel_data.width);
    uint16_t height = ntohs(pixel_data.height);
    log_debug("(x_position,y_position,width,height)=(" + std::to_string(x_position) +
              "," + std::to_string(y_position) +
              "," + std::to_string(width) +
              "," + std::to_string(height) + ")");
    int32_t encoding_type = pixel_data.encoding_type;
    if (encoding_type != RFB_ENCODING_RAW) {
        log_debug("unexpected encoding_type:" + std::to_string(encoding_type));
        return false;
    }
    uint8_t bits_per_pixel = this->pixel_format.bits_per_pixel;
    uint8_t bytes_per_pixel = bits_per_pixel / 8;
    log_debug("---pixel_format---");
    log_debug("bits_per_pixel:"   + std::to_string(bits_per_pixel));
    log_debug("bytes_per_pixel:"   + std::to_string(bytes_per_pixel));
    log_debug("------------------");

    uint32_t total_pixel_count = width * height;
    uint32_t total_pixel_bytes = total_pixel_count * bits_per_pixel / 8;
    log_debug("total_pixel_count:" + std::to_string(total_pixel_count));
    log_debug("expected total_pixel_bytes:" + std::to_string(total_pixel_bytes));

    uint32_t total_recv = 0;
    while (total_recv < total_pixel_bytes) {
        memset(buf, 0, sizeof(buf));
        if (total_pixel_bytes - total_recv < sizeof(buf)) {
            length = recv(this->sockfd, buf, total_pixel_bytes - total_recv, 0);
        } else {
            length = recv(this->sockfd, buf, sizeof(buf), 0);
        }
        if (length < 0) {
            return false;
        }
        //log_debug("recv:" + std::to_string(length));
        total_recv += length;
        for (int i = 0; i < length; i+=bytes_per_pixel) {
            // uint32_t here is just for container of 4bytes, no need to ntohl()
            uint32_t pixel = 0;
            memmove(&pixel, &buf[i], sizeof(pixel));
            this->image_buf.push_back(pixel);
        }
    }
    log_debug("total_recv reached total_pixel_bytes:" + std::to_string(total_recv));
    log_debug("image_buf size:" + std::to_string(this->image_buf.size()));

    return true;
}

bool vnc_client::draw_image()
{
    uint8_t bits_per_pixel   = this->pixel_format.bits_per_pixel;
    uint8_t depth            = this->pixel_format.depth;
    uint8_t big_endian_flag  = this->pixel_format.big_endian_flag;
    uint8_t true_colour_flag = this->pixel_format.true_colour_flag;
    uint16_t red_max         = ntohs(this->pixel_format.red_max);
    uint16_t green_max       = ntohs(this->pixel_format.green_max);
    uint16_t blue_max        = ntohs(this->pixel_format.blue_max);
    uint8_t red_shift        = this->pixel_format.red_shift;
    uint8_t green_shift      = this->pixel_format.green_shift;
    uint8_t blue_shift       = this->pixel_format.blue_shift;

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

    cv::Mat image = cv::Mat(this->height, this->width, CV_8UC3, cv::Scalar(0, 0, 0));
    log_debug(std::to_string(this->width) + "x" + std::to_string(this->height));
    for (int y = 0; y < this->height; y++) {
        for (int x = 0; x < this->width; x++) {
            uint32_t pixel = this->image_buf[this->width * y + x];
            uint8_t red = ((pixel >> red_shift) & red_max);
            uint8_t green = ((pixel >> green_shift) & green_max);
            uint8_t blue = ((pixel >> blue_shift) & blue_max);
            /*
            log_debug("(R,G,B)=(" + std::to_string(red) +
                      "," + std::to_string(green) +
                      "," + std::to_string(blue) + ")");
            */
            rectangle(image, cv::Point(x, y), cv::Point(x+1, y+1), cv::Scalar(blue, green, red), -1, cv::LINE_AA);
        }
    }
    cv::imencode(".jpeg", image, this->jpeg_buf);
    return true;
}

void vnc_client::clear_buf()
{
    this->image_buf.clear();
    this->jpeg_buf.clear();
}

std::vector<uint8_t> vnc_client::get_jpeg_buf()
{
    return this->jpeg_buf;
}

uint16_t vnc_client::get_width()
{
    return this->width;
}

uint16_t vnc_client::get_height()
{
    return this->height;
}
