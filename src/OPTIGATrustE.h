/*
* Copyright (c) 2017, Infineon Technologies AG
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* 1.  Redistributions of source code must retain the above copyright notice,
*     this list of conditions and the following disclaimer.
*
* 2.  Redistributions in binary form must reproduce the above copyright
*     notice, this list of conditions and the following disclaimer in the
*     documentation and/or other materials provided with the distribution.
*
* 3.  Neither the name of the copyright holder nor the names of its contributors
*     may be used to endorse or promote products derived from this software
*     without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*
*/

#ifndef OPTIGATRUSTE_H_
#define OPTIGATRUSTE_H_

#include "Arduino.h"

extern "C"
{
#include "util/ifx_i2c/ifx_i2c_transport_layer.h"
#include <string.h> // memcpy
#include "util/ifx_i2c/ifx_i2c_hal.h"
#include "util/ifx_i2c/ifx_i2c_config.h"
}
#include "Wire.h"



/**
 * @defgroup ifx_optiga_library Infineon OPTIGA Trust E Command Library
 * @{
 * @ingroup ifx_optiga
 *
 * @brief Module for application-level commands for Infineon OPTIGA Trust E.
 */

class OPTIGATrustE
{
public:
    //constructor
    OPTIGATrustE();

    //deconstructor
    ~OPTIGATrustE();

    /**
     *
     * This function initializes the Infineon OPTIGA Trust E command library and
     * sends the 'open application' command to the device. This opens the communicatino
     * channel to the Optiga Trust E, so that you can carry out different operations
     *
     * @retval  IFX_I2C_STACK_SUCCESS  If function was successful.
     * @retval  IFX_I2C_STACK_ERROR    If the operation failed.
     */
    uint16_t begin(void);

    /**
     *
     * This function initializes the Infineon OPTIGA Trust E command library and
     * sends the 'open application' command to the device. This opens the communicatino
     * channel to the Optiga Trust E, so that you can carry out different operations
     *
     * @param[in]  CustomWire       Reference to a custom TwoWire object used with the Optiga.
     *
     * @retval  IFX_I2C_STACK_SUCCESS  If function was successful.
     * @retval  IFX_I2C_STACK_ERROR    If the operation failed.
     */
    uint16_t begin(TwoWire& CustomWire);

    /**
     *
     * This function resets the Infineon OPTIGA Trust E. This helps to recover the connection 
     * to the optiga once it got lost. (Indicator: IFX_I2C_STACK_ERROR is returned by any other function)
     *
     * @retval  IFX_I2C_STACK_SUCCESS  If function was successful.
     * @retval  IFX_I2C_STACK_ERROR    If the operation failed.
     */
	uint16_t reset(void);
	
    /**
     *
     * @brief  Ends communication with the Optiga Trust E.
     *
     * @retval  IFX_I2C_STACK_SUCCESS  If function was successful.
     * @retval  IFX_I2C_STACK_ERROR    If the operation failed.
     */
    void end(void);

    /**
     * @brief Get a random number.
     *
     * The function retrieves a cryptographic-quality random number
     * from the OPTIGA device. This function can be used as entropy
     * source for various security schemes.
     *
     * @param[in]  length       Length of the random number (range 8 to 256).
     * @param[out] p_random     Buffer to store the data.
     *
     * @retval  IFX_I2C_STACK_SUCCESS  If function was successful.
     * @retval  IFX_I2C_STACK_ERROR    If the operation failed.
     */
    uint16_t getRandom(uint16_t length, uint8_t p_random[]);

    /**
     * @brief Get the Infineon OPTIGA Trust E device certificate.
     *
     * The function retrieves the public X.509 certificate stored in the
     * Infineon OPTIGA Trust E device.
     * This certificate and the contained public key can be used to verify a signature from the device.
     * In addition, the receiver of the certificate can verify the chain of trust
     * by validating the issuer of the certificate and the issuer's signature on it.
     *
     * @param[out] pp_cert      Pointer to the buffer that will contain the output.
     * @param[out] p_length     Pointer to the variable that will contain the length.
     *
     * @retval  IFX_I2C_STACK_SUCCESS If function was successful.
     * @retval  IFX_I2C_STACK_ERROR If the operation failed.
     */
    uint16_t getCertificate(uint8_t pp_cert[], uint32_t& p_length);

    /**
     * @brief Set the authentication scheme.
     *
     * This function sets the authentication scheme for the OPTIGA device.
     * Currently only the ECDSA with the elliptic curve SECP256R1 and
     * hash algorithm SHA256 is supported.
     *
     * @attention This function must be called once before calling @ref optiga_sign
     *
     * @retval  IFX_I2C_STACK_SUCCESS If function was successful.
     * @retval  IFX_I2C_STACK_ERROR If the operation failed.
     */
    uint16_t setAuthScheme(void);


    /**
     * @brief Sign a message using the OPTIGA device.
     *
     * The function creates a signature using the scheme selected with @ref
     * optiga_set_auth_scheme. This function can be used to implement device
     * or brand authentication schemes in applications.
     *
     * Your message length has to be equal to 16, otherwise the function will return IFX_I2C_STACK_ERROR
     *
     * @param[in]  p_message        Pointer to the buffer containing the message to be signed.
     * @param[in]  message_length   Length of the message.
     * @param[out] pp_signature     Pointer to the buffer that will contain the signature.
     *
     * @param[out] p_signature_len  Pointer to the variable which will contain the signature length.
     *
     * @retval  IFX_I2C_STACK_SUCCESS If function was successful.
     * @retval  IFX_I2C_STACK_ERROR If the operation failed.
     */
    uint16_t getSignature(uint8_t p_message[], uint16_t message_length,
                          uint8_t pp_signature[], uint32_t& p_signature_len);


    /**
     * This function returns the Global Life cycle status. Default value 0x07.
     * Lcsg has 4 states:
     * 1. Creation state with the value 0x01
     * 2. Initialization state with value of 0x03
     * 3. Operational state with the value of 0x07
     * 4. Termination state with the value of 0x15
     *
     * Changing Lcsg values only goes in one direction, from a lower value to a higher value.
     *
     * The life cycle status allows the device to identify the different logical security states of the use of the device,
     * application and other objects in the device.
     *
     * responseBuffer[out]      Pointer where the value will be stored
     * responseLength[out]      Pointer where the length of the value is stored
     *
     * @retval  IFX_I2C_STACK_SUCCESS If function was successful.
     * @retval  IFX_I2C_STACK_ERROR If the operation failed.
     */
    uint16_t getLcsg(uint8_t responseBuffer[], uint32_t& responseLength);

    /**
     * This function returns the Coprocessor UID value. Length is 27, where
     * First 25 bytes is the unique hardware identifier
     * Last 2 bytes is the Embedded Software Build Number BCD Coded
     *
     * responseBuffer[out]      Pointer where the value will be stored
     * responseLength[out]      Pointer where the length of the value is stored
     *
     * @retval  IFX_I2C_STACK_SUCCESS If function was successful.
     * @retval  IFX_I2C_STACK_ERROR If the operation failed.
     */
    uint16_t getCoprocessorId(uint8_t responseBuffer[], uint32_t& responseLength);

    /**
     * This function returns the Global Security status. Default value 0x00
     *
     * responseBuffer[out]      Pointer where the value will be stored
     * responseLength[out]      Pointer where the length of the value is stored
     * @retval  IFX_I2C_STACK_SUCCESS If function was successful.
     *
     * @retval  IFX_I2C_STACK_ERROR If the operation failed.
     */
    uint16_t getGlobalSecurityStatus(uint8_t responseBuffer[], uint32_t& responseLength);

    /**
     * This function returns the sleep mode activation delay, which is the holds the delay time in milliseconds starting from the last
     *  communication until the OPTIGA™ Trust E enters its power saving sleep mode.
     *
     *  Default value 0x14.
     *
     * responseBuffer[out]      Pointer where the value will be stored
     * responseLength[out]      Pointer where the length of the value is stored
     *
     * @retval  IFX_I2C_STACK_SUCCESS If function was successful.
     * @retval  IFX_I2C_STACK_ERROR If the operation failed.
     */
    uint16_t getSleepModeActivationDelay(uint8_t responseBuffer[], uint32_t& responseLength);

    /**
     * This function returns the current limitation, which holds the maximum value of current allowed to be consumed by the OPTIGA™
     *  Trust E across all operating conditions.
     *
     *  Default value 0x09
     *
     * responseBuffer[out]      Pointer where the value will be stored
     * responseLength[out]      Pointer where the length of the value is stored
     *
     *
     * @retval  IFX_I2C_STACK_SUCCESS If function was successful.
     * @retval  IFX_I2C_STACK_ERROR If the operation failed.
     */
    uint16_t getCurrentLimitation(uint8_t responseBuffer[], uint32_t& responseLength);

    /**
     * This function returns the security event counter. Default value 0x09
     *
     * responseBuffer[out]      Pointer where the value will be stored
     * responseLength[out]      Pointer where the length of the value is stored
     *
     * @retval  IFX_I2C_STACK_SUCCESS If function was successful.
     * @retval  IFX_I2C_STACK_ERROR If the operation failed.
     */
    uint16_t getSecurityEventCounter(uint8_t responseBuffer[], uint32_t& responseLength);

    /**
     * This function returns the Application Life Cycle Status. Default value 0x01
     *
     * * Lcsg has 3 states:
     * 1. Creation state with the value 0x01
     * 2. Initialization state with value of 0x03
     * 3. Operational state with the value of 0x07
     *
     * Changing Lcsg values only goes in one direction, from a lower value to a higher value.
     *
     * The life cycle status allows the device to identify the different logical security states of the use of the device,
     * application and other objects in the device.
     *
     * responseBuffer[out]      Pointer where the value will be stored
     * responseLength[out]      Pointer where the length of the value is stored
     *
     * @retval  IFX_I2C_STACK_SUCCESS If function was successful.
     * @retval  IFX_I2C_STACK_ERROR If the operation failed.
     */
    uint16_t getLcsa(uint8_t responseBuffer[], uint32_t& responseLength);

    /**
     * Lcsg has 4 states:
     * 1. Creation state with the value 0x01
     * 2. Initialization state with value of 0x03
     * 3. Operational state with the value of 0x07
     * 4. Termination state with the value of 0x15
     *
     * Changing Lcsg values only goes in one direction, from a lower value to a higher value.
     *
     * The life cycle status allows the device to identify the different logical security states of the use of the device,
     * application and other objects in the device.
     *
     * responseBuffer[out]      Pointer where the value will be stored
     * responseLength[out]      Pointer where the length of the value is stored
     *
     * @retval  IFX_I2C_STACK_SUCCESS If function was successful.
     * @retval  IFX_I2C_STACK_ERROR If the operation failed.
     */
    uint16_t getAppSecurityStatus(uint8_t responseBuffer[], uint32_t& responseLength);

    /**
     * This function returns the last error code.
     *
     * responseBuffer[out]      Pointer where the value will be stored
     * responseLength[out]      Pointer where the length of the value is stored
     *
     * @retval  IFX_I2C_STACK_SUCCESS If function was successful.
     * @retval  IFX_I2C_STACK_ERROR If the operation failed.
     */
    uint16_t getLastErrorCodes(uint8_t responseBuffer[], uint32_t& responseLength);

    /**
     * This function sets the Global Life Cycle Status.
     *
     * * Lcsg has 4 states:
     * 1. Creation state with the value 0x01
     * 2. Initialization state with value of 0x03
     * 3. Operational state with the value of 0x07
     * 4. Termination state with the value of 0x15
     *
     * Changing Lcsg values only goes in one direction, from a lower value to a higher value.
     *
     * The life cycle status allows the device to identify the different logical security states of the use of the device,
     * application and other objects in the device.
     *
     * dataToWrite[in]      Pointer where the value that will be set
     *
     * @retval  IFX_I2C_STACK_SUCCESS If function was successful.
     * @retval  IFX_I2C_STACK_ERROR If the operation failed.
     */
    uint16_t setLcsg(uint8_t dataToWrite[]);

    /**
     * This function sets the Global Security Status
     *
     * dataToWrite[in]      Pointer where the value that will be set
     *
     * @retval  IFX_I2C_STACK_SUCCESS If function was successful.
     * @retval  IFX_I2C_STACK_ERROR If the operation failed.
     */
    uint16_t setGlobalSecurityStatus(uint8_t dataToWrite[]);

    /**
     * This function sets the sleep mode activation delay. Valid values are 0x14 - 0xFF or 20 - 255
     *
     * dataToWrite[in]      Pointer where the value that will be set
     *
     * @retval  IFX_I2C_STACK_SUCCESS If function was successful.
     * @retval  IFX_I2C_STACK_ERROR If the operation failed.
     */
    uint16_t setSleepModeActivationDelay(uint8_t dataToWrite[]);

    /**
     * This function sets the sleep mode activation delay. Valid values are 0x09 - 0x0F or 9 - 15
     *
     * dataToWrite[in]      Pointer where the value that will be set
     *
     * @retval  IFX_I2C_STACK_SUCCESS If function was successful.
     * @retval  IFX_I2C_STACK_ERROR If the operation failed.
     */
    uint16_t setCurrentLimitation(uint8_t dataToWrite[]);

    /**
     * This function sets the Application Life Cycle Status.
     *
     * * Lcsa has 3 states:
     * 1. Creation state with the value 0x01
     * 2. Initialization state with value of 0x03
     * 3. Operational state with the value of 0x07
     *
     * Changing Lcsa values only goes in one direction, from a lower value to a higher value.
     *
     * The life cycle status allows the device to identify the different logical security states of the use of the device,
     * application and other objects in the device.
     *
     * dataToWrite[in]      Pointer where the value that will be set
     *
     * @retval  IFX_I2C_STACK_SUCCESS If function was successful.
     * @retval  IFX_I2C_STACK_ERROR If the operation failed.
     */
    uint16_t setLcsa(uint8_t dataToWrite[]);

    /**
     * This function sets the application security status.
     *
     * dataToWrite[in]      Pointer where the value that will be set
     *
     * @retval  IFX_I2C_STACK_SUCCESS If function was successful.
     * @retval  IFX_I2C_STACK_ERROR If the operation failed.
     */
    uint16_t setAppSecurityStatus(uint8_t dataToWrite[]);

    /**
     * Sets the device public key. There are restrictions built into the Optiga Chip about when you can change the certificate, so will not succeed everytime.
     *
     * dataToWrite[in]      Pointer where the value that will be set
     * length[in]           Length of the dataToWrite
     *
     */
    uint16_t setCertificate(uint8_t dataToWrite[], uint32_t length);

private:
	/**
	 * This function creates the header of length 4, which includes the command, param and the length of the data
	 */
	void CreateHeader(uint8_t* header, uint8_t command, uint8_t param,uint16_t payload_len);

	/**
	 * This function sends the apdu to the transport layer, which deals with the communication with the Optiga Trust E.
	 * At the end of the operation, the handler is called.
	 */
	uint16_t SendApdu(uint8_t* data, uint16_t length);
    /**
     * This function is a generalized version of the function used to get the values stored in the various data structures, where permitted
     */
    uint16_t generalGetFunction(uint8_t* responseBuffer, uint32_t& responseLength, uint8_t tag, uint8_t OID);

    /**
     * This function is a generalized version of the function used to set the values stored in the various data structures, where permitted
     */
    uint16_t generalSetFunction(uint8_t* dataToSet, uint32_t length, uint8_t tag, uint8_t OID);


};
/**
* @}
*/


#endif /* OPTIGATRUSTE_H_ */
