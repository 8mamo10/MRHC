#ifndef __RFB_PROTOCOL_H__
#define __RFB_PROTOCOL_H__

const uint8_t RFB_PROTOCOL_VERSION_3_3[] = "RFB 003.003\n";
const uint8_t RFB_PROTOCOL_VERSION_3_7[] = "RFB 003.007\n";
const uint8_t RFB_PROTOCOL_VERSION_3_8[] = "RFB 003.008\n";
const uint8_t RFB_SECURITY_TYPE_VNC_AUTH     = 0x02;
const uint16_t RFB_VNC_AUTH_CHALLENGE_LENGTH = 16;
const uint8_t RFB_AUTH_RESULT_OK            = 0x00;
const uint8_t RFB_AUTH_RESULT_FAILED        = 0x01;
const uint8_t RFB_SHARED_FLAG_ON            = 0x01;
const uint8_t RFB_SHARED_FLAG_OFF           = 0x00;
const uint8_t RFB_INCREMENTAL_OFF           = 0x00;
const uint8_t RFB_INCREMENTAL_ON            = 0x01;
const uint8_t RFB_MESSAGE_TYPE_FRAME_BUFFER_UPDATE_REQUEST = 0x03;

typedef struct pixel_format {
    uint8_t bits_per_pixel;
    uint8_t depth;
    uint8_t big_endian_flag;
    uint8_t true_colour_flag;
    uint16_t red_max;
    uint16_t green_max;
    uint16_t blue_max;
    uint8_t red_shift;
    uint8_t green_shift;
    uint8_t blue_shift;
    uint8_t padding[3];
} pixel_format_t;

typedef struct server_init {
    uint16_t frame_buffer_width;
    uint16_t frame_buffer_height;
    pixel_format_t pixel_format;
    uint32_t name_length;
    uint8_t name_string[BUF_SIZE];
} server_init_t;

typedef struct frame_buffer_update_request {
    uint8_t message_type;
    uint8_t incremental;
    uint16_t x_position;
    uint16_t y_position;
    uint16_t width;
    uint16_t height;
} frame_buffer_update_request_t;

#endif
