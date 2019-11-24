#include "uart.h"
#define F_CPU 16000000UL        //Says to the compiler which is our clock frequency, permits the delay functions to be very accurate
#include "avr/io.h"             //General definitions of the registers values
#include "avr/interrupt.h"

#define UART_INC(var) ( var++ % UART_BUFFER_LENGTH )
#define UART_RX_PENDING ( _rx_offset >= _rx_next ? _rx_offset - _rx_next : UART_BUFFER_LENGTH - _rx_next + _rx_offset )
#define UART_TX_PENDING ( _tx_offset >= _tx_next ? _tx_offset - _tx_next : UART_BUFFER_LENGTH - _tx_next + _tx_offset )
#define UART_GETBYTE ( _rx_buffer[UART_INC(_rx_next)] )
#define UART_WRITEBYTE( var ) _tx_buffer[UART_INC(_tx_offset)] = var
#define UART_TX_START if (( UCSR1A &( 1 << UDRE1 ))) UDR1 = _tx_buffer[ UART_INC( _tx_next ) ]

uint8_t _tx_buffer[UART_BUFFER_LENGTH];
uint8_t _rx_buffer[UART_BUFFER_LENGTH];
uint16_t _tx_offset;
uint16_t _rx_offset;
uint16_t _tx_next;
uint16_t _rx_next;
uint16_t _tx_lost;
uint16_t _rx_lost;
uint16_t _rx_length;

GUart GComm;

ISR(UART_RX_INT_VECT)
{
  _rx_buffer[UART_INC(_rx_offset)] = UART_IO_BYTE;
  if (_rx_next==_rx_offset){
    _rx_lost++;
    UART_INC(_rx_next);
  }
}

ISR(UART_TX_INT_VECT)
{
  if(UART_TX_PENDING) UART_IO_BYTE = _tx_buffer[UART_INC(_tx_next)];
}


void GUart::begin (uint16_t baud_rate){
  uint16_t prescaller = (((F_CPU / (baud_rate * 16UL))) - 1);
  UART_BAUDRATE_H = (uint8_t)(prescaller>>8);
  UART_BAUDRATE_L = (uint8_t)(prescaller);
  UART_UCSRB = ( 1 << UART_RX_ENABLED)| ( 1 << UART_TX_ENABLED )|( 1 << UART_RX_INT_ENABLED )|( 1 << UART_TX_INT_ENABLED );
  UART_UCSRC = UART_PARITY;
  _tx_offset = 0;
  _rx_offset = 0;
  _tx_next = 0;
  _rx_next = 0;
  _tx_lost = 0;
  _rx_lost = 0;
  _rx_length = 0;
}

uint16_t GUart::read(void *dest, uint16_t max_size){
  if ( !_rx_length && UART_RX_PENDING >= 2 ) {
    uint16_t l = UART_GETBYTE;
    uint16_t h = UART_GETBYTE;
      _rx_length = (h << 8) + l;
  }
  if( _rx_length && UART_RX_PENDING > _rx_length )
  {
    uint8_t *dest_c = (uint8_t *) dest;
    uint8_t check_sum = 0;
    for ( int index = 0 ; index < _rx_length ; index++ ){
      uint8_t byte_ = UART_GETBYTE;
      if (index <= max_size) dest_c[index] = byte_;
      check_sum = check_sum ^ byte_;
    }
    uint16_t result = _rx_length;
    _rx_length = 0;
    return check_sum == UART_GETBYTE ? result : 0;
  }
  return 0;
}

void GUart::write(const void *data, uint16_t size){
  uint8_t* buff=(uint8_t*)data;
  uint8_t cs=0;
  uint8_t* size_b=(uint8_t*) &size;
  UART_WRITEBYTE(*size_b++);
  UART_WRITEBYTE(*size_b);
  for ( uint8_t index = 0 ; index < size ; index++ ){
    UART_WRITEBYTE(buff[index]);
    cs=cs^buff[index];
  }
  UART_WRITEBYTE(cs);
  UART_TX_START;
}
