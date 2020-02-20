#ifndef __RFB_PROTOCOL_H__
#define __RFB_PROTOCOL_H__

typedef struct PixelFormat_ {
    uint8_t bitsPerPixel;
    uint8_t depth;
    uint8_t bigEndianFlag;
    uint8_t trueColourFlag;
    uint16_t redMax;
    uint16_t greenMax;
    uint16_t blueMax;
    uint8_t redShift;
    uint8_t greenShift;
    uint8_t blueShift;
    uint8_t padding[3];
} PixelFormat;

typedef struct ServerInit_ {
    uint16_t frameBufferWidth;
    uint16_t frameBufferHeight;
    PixelFormat pixelFormat;
    uint32_t nameLength;
    uint8_t nameString[];
} ServerInit;


#endif
