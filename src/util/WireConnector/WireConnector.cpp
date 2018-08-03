#include "WireConnector.h"

#ifdef __cplusplus
#include "Wire.h"

extern TwoWire *OptigaWire;

extern "C" {
#endif

void Wire_begin() {
    OptigaWire->begin();
}

void Wire_end() {
#ifdef WIRE_HAS_END
    OptigaWire->end();
#endif
}

void Wire_beginTransmission(uint8_t addr) {
    OptigaWire->beginTransmission(addr);
}


uint8_t Wire_endTransmission(uint8_t sendStop){
	return OptigaWire->endTransmission(sendStop);
}

uint8_t Wire_write(const uint8_t* buf, uint8_t size){
	return OptigaWire->write(buf, size);
}

uint8_t Wire_requestFrom(uint8_t address, uint8_t quantity, uint8_t sendStop){
	return OptigaWire->requestFrom(address, quantity, sendStop);
}

int Wire_available(void){
	return OptigaWire->available();
}

int Wire_read(void){
	return OptigaWire->read();
}

void Wire_setClock(uint32_t clk){
	OptigaWire->setClock(clk);
}

#ifdef __cplusplus
}
#endif
