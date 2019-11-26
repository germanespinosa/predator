#include "gbuffer.h"

GBuffer::GBuffer() {
    _size = 256;
    _offset = 0;
    _next = 0;
}

uint32_t GBuffer::pending() {
    if (_offset>=_next) return _offset - _next;
    return _size - _next + _offset;
}
void GBuffer::write_byte(uint8_t byte) {
    _buffer[_offset] = byte;
    _offset++;
    if (_offset >= _size) _offset = 0;
}
bool GBuffer::read_byte(uint8_t &byte) {
    if (pending()){
        byte = _buffer[_next];
        _next++;
        if (_next>=_size) _next = 0;
        return true;
    }
    return false;
}
