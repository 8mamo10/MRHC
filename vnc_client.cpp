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

    frame_buffer_update_request_t r = {};
    r.message_type = RFB_MESSAGE_TYPE_FRAME_BUFFER_UPDATE_REQUEST;
    r.incremental = RFB_INCREMENTAL_OFF;
    r.x_position = htons(0);
    r.y_position = htons(0);
    r.width = this->server_init.frame_buffer_width;
    r.height = this->server_init.frame_buffer_height;

    len = send(this->sockfd, &r, sizeof(r), 0);
    if (len < 0) {
        return false;
    }
    log_debug("send:" + std::to_string(len));

    // frame_buffer_update
    frame_buffer_update_t update = {};
    len = recv(this->sockfd, buf, sizeof(update), 0);
    if (len < 0) {
        return false;
    }
    log_debug("recv:" + std::to_string(len));
    log_xdebug(buf, len);

    memmove(&update, buf, sizeof(update));
    uint8_t message_type = update.message_type;
    if (message_type != RFB_MESSAGE_TYPE_FRAME_BUFFER_UPDATE) {
        log_debug("unexpected message_type:" + std::to_string(message_type));
        return false;
    }
    uint16_t number_of_rectangles = ntohs(update.number_of_rectangles);
    log_debug("number_of_rectangles:" + std::to_string(number_of_rectangles));

    // pixel_data
    pixel_data_t data = {};
    memset(buf, 0, sizeof(buf));
    len = recv(this->sockfd, buf, sizeof(data), 0);
    if (len < 0) {
        return false;
    }
    log_debug("recv:" + std::to_string(len));
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
    int total_pixel_bytes = width * height * (this->server_init.pixel_format.bits_per_pixel / 8);
    log_debug("expected total_pixel_bytes:" + std::to_string(total_pixel_bytes));

    if (!this->drawImage()) {
        return false;
    }
    return true;
}

std::vector<uint8_t> vnc_client::get_image_buf()
{
    return this->image_buf;
}

//bool vnc_client::drawImage(const char recv_buf[], const int recv_len)
bool vnc_client::drawImage()
{
    // sample drawing
    cv::Mat image = cv::Mat(ntohs(this->server_init.frame_buffer_height),
                            ntohs(this->server_init.frame_buffer_width),
                            CV_8UC3, cv::Scalar(0, 0, 0));
    // rectangle(image, cv::Point(10, 10), cv::Point(100, 100), cv::Scalar(128, 0, 0), -1, cv::LINE_AA);
    // rectangle(image, cv::Point(200, 200), cv::Point(300, 300), cv::Scalar(0, 128, 0), -1, cv::LINE_AA);
    // rectangle(image, cv::Point(500, 500), cv::Point(700, 700), cv::Scalar(0, 0, 128), -1, cv::LINE_AA);
    // cv::imencode(".jpeg", image, this->image_buf);

    for (int w = 0; w < image.size().width; w++) {
        for (int h = 0; h < image.size().height; h++) {
            cv::Scalar color = cv::Scalar(0, 0, 0);
            if (w < 400) {
                if (h < 300) {
                    color = cv::Scalar(255, 0, 0);
                } else {
                    color = cv::Scalar(0, 255, 0);
                }
            } else {
                if (h < 300) {
                    color = cv::Scalar(0, 0, 255);
                } else {
                    color = cv::Scalar(255, 255, 255);
                }
            }
            //rectangle(image, cv::Point(w, h), cv::Point(w+1, h+1), cv::Scalar(128, 0, 0), -1, cv::LINE_AA);
            rectangle(image, cv::Point(w, h), cv::Point(w+1, h+1), color, -1, cv::LINE_AA);
        }
    }
    cv::imencode(".jpeg", image, this->image_buf);
    return true;
}
