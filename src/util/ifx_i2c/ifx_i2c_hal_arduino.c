/*
 * ifx_i2c_wire.c
 *
 *  Created on: 11.07.2017
 *      Author: Pahari
 */


#ifdef ARDUINO

#include "ifx_i2c_hal.h"
#include "../WireConnector/WireConnector.h"
#include "Arduino.h"

#define MAX_POLLING				50

static volatile IFX_I2C_EventHandler upper_layer_event_handler = 0;
volatile IFX_Timer_Callback timer_callback              = 0;

//used in IFXOPTIGATrustE.cpp. Reduces the stack size because it is declared as an extern variable
uint8_t m_timer_done = 0;

/*
 * Used for the soft reset while initializing the handler.
 * Transmits data to the Slave
 */
void ifx_i2c_transmitWithoutHandler(uint8_t* data, uint16_t length)
{
	uint8_t wReceivedBytes = 1;
	uint16_t counterForTransmission = 0;
	do
	 {

		Wire_beginTransmission(IFX_I2C_BASE_ADDR);
		Wire_write(data, length);
		wReceivedBytes = Wire_endTransmission((uint8_t)1);
		counterForTransmission++;
	 }  while (wReceivedBytes != 0 && counterForTransmission < MAX_POLLING);
}

/*
 * Used for the soft reset while initializing the handler.
 * Receives data from the Slave
 */
bool ifx_i2c_receiveWithoutHandler(uint8_t* data, uint16_t length)
{
	uint8_t wReceivedBytes = 0;
	uint16_t wReadLen = 0;
	uint16_t counterForRecieve = 0;
	do
	{

		Wire_beginTransmission(IFX_I2C_BASE_ADDR);
		wReceivedBytes = Wire_requestFrom(IFX_I2C_BASE_ADDR, length, (uint8_t)1);
		counterForRecieve++;
	}	while(wReceivedBytes==0 && counterForRecieve < MAX_POLLING);


	while(Wire_available())
	{
	   data[wReadLen] = Wire_read();
	   wReadLen++;
	}

	if (wReadLen == length) return false;
	else return true;
}

/**
 * @brief Function to perform a software reset on optiga.
 *
 * @param  void
 */
uint16_t ifx_i2c_optiga_soft_reset(void)
{
	uint8_t rgbSoftResetData[3] = {0x88,0x00,0x00};
	uint8_t prgbStateDataReg[4] = {0x00};
	uint8_t prgbStateRegWrite[1] = { 0x82 };

	//Check if the soft reset is supported
	ifx_i2c_transmitWithoutHandler(prgbStateRegWrite,1);

	if (ifx_i2c_receiveWithoutHandler(prgbStateDataReg,4)) return IFX_I2C_STACK_ERROR;

	if(0x08 != (prgbStateDataReg[0]&0x08))
	{
		return IFX_I2C_STACK_ERROR;
	}

	ifx_i2c_transmitWithoutHandler(rgbSoftResetData,3);

	return IFX_I2C_STACK_SUCCESS;
}

/**
 * @brief Function for initializing a HAL module.
 *
 * The function initializes a HAL module.
 *
 * @param  reinit   If 1, the call shal re-initializes the HAL module if it was used before.
 *                  If 0, the module is initialized for the first time.
 * @param  handler  Event handler to propagate events to the upper layer
 */
uint16_t ifx_i2c_init(uint8_t reinit, IFX_I2C_EventHandler handler)
{
	if (reinit) {Wire_end();}

	upper_layer_event_handler = handler;

	Wire_begin();

	return ifx_i2c_optiga_soft_reset();
}

/**
 * @brief I2C transmit function to conduct an I2 write on I2C bus.
 *
 * The function conducts an I2C write on the I2C bus.
 *
 * @param  data    Pointer to buffer with data to be written to I2C slave
 * @param  length  Length of data in data buffer
 */
void ifx_i2c_transmit(uint8_t* data, uint16_t length)
{
	uint8_t wReceivedBytes = 1;
	uint16_t counterForTransmission = 0;
	//According to the protocol of the Optiga Trust E, it might require some time to turn on and respond
	do
	 {
		Wire_beginTransmission(IFX_I2C_BASE_ADDR);
		Wire_write(data, length);
		wReceivedBytes = Wire_endTransmission((uint8_t)1);
		counterForTransmission++;
	 }  while (wReceivedBytes != 0 && counterForTransmission < MAX_POLLING);

	//Go to the upper layer handler (physical layer)
	if (wReceivedBytes == 0)
	{
		upper_layer_event_handler(IFX_I2C_HAL_TX_SUCCESS);
	}
	else
	{
		upper_layer_event_handler(IFX_I2C_HAL_ERROR);
	}
}

/**
 * @brief I2C receive function to conduct an I2 read on I2C bus.
 *
 * The function conducts an I2C read on the I2C bus.
 *
 * @param  data    Pointer to buffer where received data shall be stored
 * @param  length  Number of bytes to read from I2C slave
 */
void ifx_i2c_receive(uint8_t* data, uint16_t length)
{
	uint16_t wReadLen = 0;

	uint8_t wReceivedBytes = 0;
	uint16_t counterForRecieve = 0;
//According to the protocol of the Optiga Trust E, it might require some time to turn on and respond
	do
	{
		Wire_beginTransmission(IFX_I2C_BASE_ADDR);
		wReceivedBytes = Wire_requestFrom(IFX_I2C_BASE_ADDR, length, (uint8_t)1);
		counterForRecieve++;
	}	while(wReceivedBytes == 0 && counterForRecieve < MAX_POLLING);

	if (wReceivedBytes == 0)
	{
		upper_layer_event_handler(IFX_I2C_HAL_ERROR);
	}

	else
	{
		while(Wire_available())
		{
		   data[wReadLen] = Wire_read();
		   wReadLen++;
		}

		//Go to the upper layer handler (physical layer). We have received the bytes that we needed
		if (wReadLen == length)
		{
			upper_layer_event_handler(IFX_I2C_HAL_RX_SUCCESS);
		}

		else
		{
			upper_layer_event_handler(IFX_I2C_HAL_ERROR);
		}
	}



}

/**
 * @brief Timer setup function to initialize and start a timer.
 *
 * The function delays for the time given. Then it changes the value of m_time_done, which is an extern variable used in IFXOPTIGATrustE.cpp.
 * Since this function is not recursive, it reduces the size of the stack each time it is called.
 *
 * @param  time_us            Time in microseconds after the timer expires
 * @param  callback_function  Function to be called once timer expired
 */

void ifx_timer_setup(uint16_t time_us, IFX_Timer_Callback callback_function)
{
	timer_callback = callback_function;

	delayMicroseconds(time_us);

	m_timer_done = 1;

}

#endif

