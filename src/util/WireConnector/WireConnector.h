#ifndef WIRE_CONNECTOR_H 
#define WIRE_CONNECTOR_H 

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void Wire_begin(void);

void Wire_end(void);

void Wire_beginTransmission(uint8_t);

uint8_t Wire_endTransmission(uint8_t);

uint8_t Wire_write(const uint8_t*, uint8_t);

uint8_t Wire_requestFrom(uint8_t, uint8_t, uint8_t);

int Wire_available(void);

int Wire_read(void);

void Wire_setClock(uint32_t);

#ifdef __cplusplus
}
#endif

#endif //WIRE_CONNECTOR_H
