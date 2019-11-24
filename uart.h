#ifndef __UART_H__
#define __UART_H__
#include "uart_def.h"
#include <stdint.h>
#define UART_BUFFER_LENGTH 1024

struct GUart{
    void begin(uint16_t);
    uint16_t read(void *, uint16_t);
    void write(const void *, uint16_t);
};

extern GUart GComm;
#endif
