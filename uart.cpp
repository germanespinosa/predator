#include "uart_def.h"
#include <stdint.h>

#include "gbuffer.h"
#include "gbufferconnector.h"

#define F_CPU 16000000UL        //Says to the compiler which is our clock frequency, permits the delay functions to be very accurate
#include "avr/io.h"             //General definitions of the registers values
#include "avr/interrupt.h"

#define UART_TX_START if (( UCSR1A &( 1 << UDRE1 ))){ uint8_t b; if(_tx_buffer.read_byte(b)) UART_IO_BYTE = b; }

GBuffer _tx_buffer;
GBuffer _rx_buffer;

GBufferConnector conn(_rx_buffer,_tx_buffer);


ISR(UART_RX_INT_VECT)
{
  _rx_buffer.write_byte(UART_IO_BYTE);
}

ISR(UART_TX_INT_VECT)
{
  uint8_t b;
  if(_tx_buffer.read_byte(b)) UART_IO_BYTE = b;
}

void Uart_begin (uint16_t baud_rate){
  uint16_t prescaller = (((F_CPU / (baud_rate * 16UL))) - 1);
  UART_BAUDRATE_H = (uint8_t)(prescaller>>8);
  UART_BAUDRATE_L = (uint8_t)(prescaller);
  UART_UCSRB = ( 1 << UART_RX_ENABLED)| ( 1 << UART_TX_ENABLED )|( 1 << UART_RX_INT_ENABLED )|( 1 << UART_TX_INT_ENABLED );
  UART_UCSRC = UART_PARITY;
}
