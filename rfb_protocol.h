#ifndef __RFB_PROTOCOL_H__
#define __RFB_PROTOCOL_H__

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

#endif
