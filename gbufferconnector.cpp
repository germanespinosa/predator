#include"gbufferconnector.h"

uint16_t b254(uint16_t b255){
    uint16_t s = b255 >> 8;
    b255 += s;
    if (b255 >> 8 > s) b255++;
    if (b255 % 256 == 255) b255 ++;
    return b255;
}

uint16_t b255(uint16_t b254){
    uint16_t s = b254 >> 8;
    b254 -= s;
    return b254;
}

GBufferConnector::GBufferConnector(GBuffer &rx_buffer, GBuffer &tx_buffer) :
_rx_buffer(rx_buffer), _tx_buffer(tx_buffer)
{}

void GBufferConnector::write_buffer(const void *data, uint16_t size) {
    uint8_t *buff = (uint8_t *) data;
    uint8_t check_sum = 0;
    uint16_t ns = size;
    for (uint32_t index = 0; index < size; index++) {
        if (buff[index]==255 || buff[index]==127) ns ++;
    }
    _tx_buffer.write_byte(255);
    uint16_t cs = b254(ns);
    uint8_t h = *(((uint8_t *) &cs) + 1);
    uint8_t l = *((uint8_t *) &cs);
    _tx_buffer.write_byte(l);
    _tx_buffer.write_byte(h);
    for (uint32_t index = 0; index < size; index++) {
        switch(buff[index]) {
            case 255:
                _tx_buffer.write_byte(127);
                _tx_buffer.write_byte(128);
                break;
            case 127:
                _tx_buffer.write_byte(127);
                _tx_buffer.write_byte(0);
                break;
            default:
                _tx_buffer.write_byte(buff[index]);
        }
        check_sum = check_sum ^ buff[index];
    }
    _tx_buffer.write_byte(check_sum);
}

#define CHECK_BYTE {_rx_buffer.read_byte(byte);if (byte==255){ _rx_length = 0; return 0;}}

uint16_t GBufferConnector::read_buffer(void *dest, uint16_t max_size) {
    static uint16_t _rx_length = 0;
    uint8_t byte;
    if (_rx_length == 0 && _rx_buffer.pending() >= 2) {
        _rx_buffer.read_byte(byte);
        if (byte==255) {// message starter
            if (_rx_buffer.pending() >= 2) CHECK_BYTE
            else return 0;
        }
        uint16_t l = byte;
        CHECK_BYTE
        uint16_t h = byte;
        _rx_length = b255((h << 8) + l);
    }
    if (_rx_length != 0 && _rx_buffer.pending() > _rx_length) {
        uint8_t *dest_c = (uint8_t *) dest;
        uint8_t check_sum = 0;
        int x = 0;
        for (uint32_t index = 0; index + x < _rx_length; index++) {
            CHECK_BYTE
            if (byte==127) {
                CHECK_BYTE
                byte += 127;
                x++;
            }
            if (index < max_size) dest_c[index] = byte;
            check_sum = check_sum ^ byte;
        }
        uint16_t length = _rx_length;
        _rx_length = 0;
        CHECK_BYTE
        return check_sum == byte ? length - x : 0;
    }
    return 0;
}
