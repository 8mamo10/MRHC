#ifndef __RFB_PROTOCOL_H__
#define __RFB_PROTOCOL_H__

const uint16_t RFB_BUF_SIZE = 256;

// RFB 003.003\n
const uint8_t RFB_PROTOCOL_VERSION_3_3[] = {0x52, 0x46, 0x42, 0x20, 0x30, 0x30, 0x33, 0x2e, 0x30, 0x30, 0x33, 0x0a};
// RFB 003.007\n
const uint8_t RFB_PROTOCOL_VERSION_3_7[] = {0x52, 0x46, 0x42, 0x20, 0x30, 0x30, 0x33, 0x2e, 0x30, 0x30, 0x37, 0x0a};
// RFB 003.008\n
const uint8_t RFB_PROTOCOL_VERSION_3_8[] = {0x52, 0x46, 0x42, 0x20, 0x30, 0x30, 0x33, 0x2e, 0x30, 0x30, 0x38, 0x0a};
const uint8_t RFB_SECURITY_TYPE_VNC_AUTH    = 0x02;
const uint8_t RFB_VNC_AUTH_CHALLENGE_LENGTH = 16;
const uint8_t RFB_SECURITY_RESULT_OK        = 0x00;
const uint8_t RFB_SECURITY_RESULT_FAILED    = 0x01;
const uint8_t RFB_SHARED_FLAG_ON            = 0x01;
const uint8_t RFB_SHARED_FLAG_OFF           = 0x00;
const uint8_t RFB_INCREMENTAL_OFF           = 0x00;
const uint8_t RFB_INCREMENTAL_ON            = 0x01;
const uint8_t RFB_ENCODING_RAW              = 0x00;
const uint8_t RFB_KEY_UP                    = 0x00;
const uint8_t RFB_KEY_DOWN                  = 0x01;
const uint8_t RFB_POINTER_BUTTON_LEFT       = 0x00;
const uint8_t RFB_POINTER_BUTTON_RIGHT      = 0x02;
const uint8_t RFB_POINTER_UP                = 0x00;
const uint8_t RFB_POINTER_DOWN              = 0x01;
const uint8_t RFB_MESSAGE_TYPE_SET_PIXEL_FORMAT            = 0x00;
const uint8_t RFB_MESSAGE_TYPE_SET_ENCODINGS               = 0x02;
const uint8_t RFB_MESSAGE_TYPE_FRAME_BUFFER_UPDATE_REQUEST = 0x03;
const uint8_t RFB_MESSAGE_TYPE_KEY_EVENT                   = 0x04;
const uint8_t RFB_MESSAGE_TYPE_POINTER_EVENT               = 0x05;
const uint8_t RFB_MESSAGE_TYPE_FRAME_BUFFER_UPDATE         = 0x00;
const uint8_t RFB_MESSAGE_TYPE_BELL                        = 0x02;

const uint32_t RFB_KEY_CODE_BACKSPACE = 0xff08;
const uint32_t RFB_KEY_CODE_PERIOD    = 0x002e;
const uint32_t RFB_KEY_CODE_ENTER     = 0xff0d;
const uint32_t RFB_KEY_CODE_SPACE     = 0xff80;
const uint32_t RFB_KEY_CODE_SLASH     = 0x002f;

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

typedef struct pixel_data {
    uint16_t x_position;
    uint16_t y_position;
    uint16_t width;
    uint16_t height;
    int32_t encoding_type;
    //uint8_t pixels[]; // currently only support raw encoding
} pixel_data_t;

// handshaking messages
typedef struct protocol_version {
    uint8_t values[12];
} protocol_version_t;

typedef struct supported_security_types {
    uint8_t number_of_security_types;
    uint8_t security_types[RFB_BUF_SIZE];
} supported_security_types_t;

typedef struct security_type {
    uint8_t value;
} security_type_t;

typedef struct vnc_auth_challenge {
    uint8_t values[RFB_VNC_AUTH_CHALLENGE_LENGTH];
} vnc_auth_challenge_t;

typedef struct vnc_auth_response {
    uint8_t values[RFB_VNC_AUTH_CHALLENGE_LENGTH];
} vnc_auth_response_t;

typedef struct security_result {
    uint32_t status;
} security_result_t;

// initialisation Messages
typedef struct client_init {
    uint8_t shared_flag;
} client_init_t;

typedef struct server_init {
    uint16_t frame_buffer_width;
    uint16_t frame_buffer_height;
    pixel_format_t pixel_format;
    uint32_t name_length;
    uint8_t name_string[RFB_BUF_SIZE];
} server_init_t;

// client to server messages
typedef struct set_pixel_format {
    uint8_t message_type = RFB_MESSAGE_TYPE_SET_PIXEL_FORMAT;
    uint8_t padding[3];
    pixel_format_t pixel_format;
} set_pixel_format_t;

typedef struct set_encodings {
    uint8_t message_type = RFB_MESSAGE_TYPE_SET_ENCODINGS;
    uint8_t padding;
    uint16_t number_of_encodings;
    int32_t encoding_type; // temporary only 1 slot
} set_encodings_t;

typedef struct frame_buffer_update_request {
    uint8_t message_type = RFB_MESSAGE_TYPE_FRAME_BUFFER_UPDATE_REQUEST;
    uint8_t incremental;
    uint16_t x_position;
    uint16_t y_position;
    uint16_t width;
    uint16_t height;
} frame_buffer_update_request_t;

typedef struct key_event {
    uint8_t message_type = RFB_MESSAGE_TYPE_KEY_EVENT;
    uint8_t down_flag;
    uint8_t padding[2];
    uint32_t key;
} key_event_t;

typedef struct pointer_event {
    uint8_t message_type = RFB_MESSAGE_TYPE_POINTER_EVENT;
    uint8_t button_mask;
    uint16_t x_position;
    uint16_t y_position;
} pointer_event_t;

// server to client messages
typedef struct frame_buffer_update {
    uint8_t message_type = RFB_MESSAGE_TYPE_FRAME_BUFFER_UPDATE;
    uint8_t padding;
    uint16_t number_of_rectangles;
    //pixel_data_t pixel_datas[];
} frame_buffer_update_t;

typedef struct bell {
    uint8_t message_type = RFB_MESSAGE_TYPE_BELL;
} bell_t;

#endif
