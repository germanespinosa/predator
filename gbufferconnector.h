#ifndef GCOMM_GBUFFERCONNECTOR_H
#define GCOMM_GBUFFERCONNECTOR_H
#include "gbuffer.h"
#include <stdint.h>

uint16_t b254(uint16_t b255);
uint16_t b255(uint16_t b254);

struct GBufferConnector {
public:
    GBufferConnector(GBuffer &, GBuffer &);

    uint16_t read_buffer(void *, uint16_t);

    void write_buffer(const void *, uint16_t);

private:
    GBuffer &_rx_buffer;
    GBuffer &_tx_buffer;
};
#endif
