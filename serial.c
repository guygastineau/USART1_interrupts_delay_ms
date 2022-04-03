#include "serial.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>

// PC0 - tx; PC1 - rx
// USARTROUTEA 00

// Calculation for baud register for async normal mode only.
// This is also the number of bytes per second for the setting.
#define BAUDGEN(BAUD) ((float)((64 * F_CPU)/(S * (float)(BAUD))) + 0.5)

enum { ASYNC = 0 /* default */, SYNC = 0x40, IRCOM = 0x80, MSPI = 0xC0 };
enum { PARITY_DISABLED = 0 /*default */, PARITY_EVEN = 0x20, PARITY_ODD = 0x30 };
enum { STOPBIT_1 = 0 /* default */, STOPBIT_2 = 8 };
enum { BIT5 = 0, BIT6 = 1, BIT7 = 2, BIT8 = 3 /* default */,
	   BIT9L = 6, BIT9H = 7 };

enum { RXEN = 0x80 };
enum { TXEN = 0x40 };
enum { SFDEN = 0x10 };
enum { ODME = 8 };
enum { RXMODE_NORMAL = 0, RXMODE_CLK2X = 2, RXMODE_GENAUTO = 4,
	   RXMODE_LINAUTO = 6 };
enum { MPCM = 1 };

enum { RXCIF = 0x80, TXCIF= 0x40, DREIF = 0x20, RXSIF = 0x10, ISFIF = 8,
	   BDF = 2, WFB = 1 };

enum { RXCIE = RXCIF, TXCIE = TXCIF, DREIE = DREIF };

static const uint8_t *txBuf;
static const uint8_t *txBufEnd;
static volatile bool txBusy;

void serialInit(unsigned long baud)
{
  cli();
  USART1.BAUD = (uint16_t)BAUDGEN(baud);
  // Set operating mode.
  USART1.CTRLC = ASYNC | PARITY_DISABLED | STOPBIT_1 | BIT8;
  PORTC.DIRSET = PIN0_bm;
  PORTC.DIRCLR = PIN1_bm;
  // Enable serial
  USART1.CTRLB |= TXEN | RXEN;
  // Enable desired UART interrupts.
  USART1.CTRLA = DREIE;
  txBusy = false;
  sei();
}

void serialSendByte(uint8_t data)
{
  while (!(USART1.STATUS & DREIF));
  USART1.TXDATAL = data;
}

void serialSendBuff(const uint8_t *data, uint16_t length)
{
  if (length == 0) {
	return;
  }

  serialWaitTx();
  txBusy = true;
  txBuf = data;
  txBufEnd = &data[length];
  USART1.STATUS = DREIF;

}

void serialWaitTx(void)
{
  while (txBusy){
	_delay_ms(1);
  }
}

ISR(USART1_DRE_vect)
{
  if (txBuf < txBufEnd) {
	USART1.TXDATAL = *txBuf++;
  } else {
	txBusy = false;
  }
}
