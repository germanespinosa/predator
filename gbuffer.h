#ifndef GCOMM_GBUFFER_H
#define GCOMM_GBUFFER_H

#include <stdint.h>

struct GBuffer {
    GBuffer();

    void write_byte(uint8_t);

    uint32_t pending();

    bool read_byte(uint8_t&);

//private:
    uint8_t _buffer[256];
    uint32_t _next;
    uint32_t _offset;
    uint32_t _size;
};

#endif //GCOMM_GBUFFER_H
