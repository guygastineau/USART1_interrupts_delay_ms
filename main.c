#include "serial.h"

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/xmega.h>

#define OSCHFCTRLA_BYTE(RUNSTDBY, FRQSEL, AUTOTUNE) \
  (((RUNSTDBY << 7) | (((FRQSEL) & 0x0F) << 2) | ((AUTOTUNE) & 1)))

enum { CLKFRQ_1M = 0, CLKFRQ_2M, CLKFRQ_3M, CLKFRQ_4M,
	   CLKFRQ_8M = 5, CLKFRQ_12M, CLKFRQ_16M, CLKFRQ_20M, CLKFRQ_24M };

void setClockControl(void)
{
  // Set frequency
  _PROTECTED_WRITE(CLKCTRL.OSCHFCTRLA, OSCHFCTRLA_BYTE(0, CLKFRQ_16M, 0));
  // Lock it.
  _PROTECTED_WRITE(CLKCTRL.MCLKLOCK, 1);
}

static const uint8_t *hello = (const uint8_t *)"Hello, World!";

int main (void)
{
  setClockControl();
  serialInit(115200);
  PORTA.DIRSET |= PIN7_bm;
  sei();
  while (1) {
	serialSendBuff(hello, 13);
	serialWaitTx();

	PORTA.OUTSET = PIN7_bm;
	_delay_ms(100);
	PORTA.OUTCLR  = PIN7_bm;
	_delay_ms(100);
  }
}
