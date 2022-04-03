#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>

#ifndef F_CPU
#warning "serial.h: F_CPU not defined!"
#endif

// # of samples (normal mode for async)
#define S 16UL

// ***                                                              ***
// *** Caller must disable interrupts first and then re-enable them ***
// ***                                                              ***
// User should set their own interrupt vector (or maybe we can just pass
// a callback in future?).  We could also use a flag to controll using
// a default interrupt or not.
void serialInit(unsigned long baud);
void serialSendByte(uint8_t data);
void serialSendBuff(const uint8_t *data, uint16_t length);
void serialWaitTx(void);

#endif // SERIAL_H
