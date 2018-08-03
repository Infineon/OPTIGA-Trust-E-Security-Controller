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

#include "OPTIGATrustE.h"

//these variables can be found in ifx_i2c_hal.c
extern uint8_t m_timer_done;
extern volatile IFX_Timer_Callback timer_callback;
extern uint16_t ifx_i2c_optiga_soft_reset(void);

// Command headers
#define OPTIGA_CMD_HEADER_LEN                   4
#define HEADER_SPACE                            0x00, 0x00, 0x00, 0x00
#define OPTIGA_CMD_FLAG_FLUSH_LAST_ERROR        0x80

// Command status codes
#define OPTIGA_CMD_STATUS_SUCCESS               0x00

// Command Open Application
#define OPTIGA_CMD_OPEN_APPLICATION             0x70
#define APP_ID                                  0xD2, 0x76, 0x00, 0x00, 0x04, 0x47, 0x65, 0x6E, \
                                                0x41, 0x75, 0x74, 0x68, 0x41, 0x70, 0x70, 0x6C

// Command Set Auth Scheme
#define OPTIGA_CMD_SET_AUTH_SCHEME              0x10
#define OPTIGA_AUTH_ECDSA_SECP256R1_SHA256      0x91
#define OID_PRIVATE_KEY                         OPTIGA_OID_TAG, OPTIGA_OID_PRIVATE_KEY

// Command Set Auth Message
#define OPTIGA_CMD_SET_AUTH_MSG                 0x19
#define OPTIGA_PARAM_CHALLENGE                  0x01
#define OPTIGA_AUTH_MSG_LEN                     16

// Command Get Auth Message
#define OPTIGA_CMD_GET_AUTH_MSG                 0x18
#define OPTIGA_PARAM_SIGNATURE                  0x02

// Command Get Random
#define OPTIGA_CMD_GET_RANDOM                   0x0C

// Command Get Data Object
#define OPTIGA_CMD_GET_DATA_OBJECT              0x01
#define OPTIGA_PARAM_READ_DATA                  0x00
#define OPTIGA_CMD_SET_DATA_OBJECT              0x02
#define OPTIGA_PARAM_WRITE_DATA                 0x00
#define WRITE_OFFSET                            0x00,0x00
// Data structure object identifiers
#define OPTIGA_OID_TAG                          0xE0
#define OPTIGA_OID_PRIVATE_KEY                  0xF0
#define OPTIGA_OID_INFINEON_CERT                0xE0
#define OPTIGA_OID_PROJECT_CERT                 0xE1
#define OPTIGA_APP_TAG                          0xF1

/// Global Life Cycle State
#define     LCS_G                               0xC0
/// Global Security Status
#define     SECURITY_STATUS_G                   0xC1
/// Coprocessor UID
#define COPROCESSOR_UID                         0xC2
/// Global Life Cycle State
#define     SLEEP_MODE_ACTIVATION_DELAY         0xC3
/// Current limitation
#define     CURRENT_LIMITATION                  0xC4
/// Security Event Counter
#define     SECURITY_EVENT_COUNTER              0xC5
//Infineon Certificate
#define     OID_CERTIFICATE                     OPTIGA_OID_TAG, OPTIGA_OID_INFINEON_CERT
/// Project-Specific device Public Key Certificate
#define     DEVICE_PUBKEY_CERT_PRJSPC           OPTIGA_OID_TAG, OPTIGA_OID_PROJECT_CERT
/// First Device Private Key
#define     FIRST_DEVICE_PRIKEY                 0xF0
/// Application Life Cycle Status
#define     LCS_A                               0xC0
/// Application Security Status
#define     SECURITY_STATUS_A                   0xC1
/// Error codes
#define     ERROR_CODES                         0xC2

// Members to use library in blocking mode
static volatile uint8_t   m_ifx_i2c_busy = 0;
static volatile uint8_t   m_ifx_i2c_status;
static volatile uint8_t* m_optiga_rx_buffer;
static volatile uint16_t  m_optiga_rx_len;

// Wire used by Optiga Trust E
TwoWire* OptigaWire = &Wire;

/**
 * The event handler, which is called when the transport layer is done communicating with the lower levels and the operation has been carried out
 */
static void ifx_i2c_tl_event_handler(uint8_t event, uint8_t* data, uint16_t data_len)
{
    m_optiga_rx_buffer = data;
    m_optiga_rx_len = data_len;
    m_ifx_i2c_status = event;
    m_ifx_i2c_busy = 0;
}

OPTIGATrustE::OPTIGATrustE()
{

}

OPTIGATrustE::~OPTIGATrustE()
{

}

/**
 * This function creates the header of length 4, which includes the command, param and the length of the data
 */
void OPTIGATrustE::CreateHeader(uint8_t* header, uint8_t command, uint8_t param,
                               uint16_t payload_len)
{
    header[0] = command | OPTIGA_CMD_FLAG_FLUSH_LAST_ERROR;
    header[1] = param;
    header[2] = payload_len >> 8;
    header[3] = payload_len;
}

/**
 * This function sends the apdu to the transport layer, which deals with the communication with the Optiga Trust E.
 * At the end of the operation, the handler is called.
 */
uint16_t OPTIGATrustE::SendApdu(uint8_t* data, uint16_t length)
{
    uint16_t response_len = 0;

    m_ifx_i2c_busy = 1;
    if (ifx_i2c_tl_transceive(data, length))
    {
        return IFX_I2C_STACK_ERROR;
    }


    /**
     *  Due to the recursive implementation of transport layer, datalink layer and physical layer,
     *  the process below will reduce the size of the stack each time the ifx_timer_setup function is called in the ifx_i2c_hal.c.
     */
    while (m_ifx_i2c_busy)
    {
        //Wait
        if (m_timer_done)
        {
            m_timer_done = 0;
            if (timer_callback)
            {
                timer_callback();
            }
        }
    }

    if (m_optiga_rx_len < OPTIGA_CMD_HEADER_LEN)
    {
        return IFX_I2C_STACK_ERROR;
    }
    if (m_optiga_rx_buffer[0] != OPTIGA_CMD_STATUS_SUCCESS)
    {
        return IFX_I2C_STACK_ERROR;
    }

    response_len = (m_optiga_rx_buffer[2] << 8) | m_optiga_rx_buffer[3];
    if (OPTIGA_CMD_HEADER_LEN + response_len != m_optiga_rx_len)
    {
        return IFX_I2C_STACK_ERROR;
    }

    if (m_ifx_i2c_status == IFX_I2C_TL_EVENT_SUCCESS)
    {
        return IFX_I2C_STACK_SUCCESS;
    }
    else
    {
        return IFX_I2C_STACK_ERROR;
    }
}

uint16_t OPTIGATrustE::begin(void)
{
    return begin(Wire);
}

uint16_t OPTIGATrustE::begin(TwoWire& CustomWire)
{
    uint8_t apdu[] = { HEADER_SPACE, APP_ID };
	
    // Set global wire used with Optiga
    OptigaWire = &CustomWire;

    CreateHeader(apdu, OPTIGA_CMD_OPEN_APPLICATION, 0x00,
                       sizeof(apdu) - OPTIGA_CMD_HEADER_LEN);

    if (ifx_i2c_tl_init(ifx_i2c_tl_event_handler) != IFX_I2C_STACK_SUCCESS)
    {
        return IFX_I2C_STACK_ERROR;
    }
    return SendApdu(apdu, sizeof(apdu));
}

uint16_t OPTIGATrustE::reset(void)
{
	if(OptigaWire == NULL)
	{
        return IFX_I2C_STACK_ERROR;
	}
	
    end();
    return begin(*OptigaWire);
}

void OPTIGATrustE::end(void)
{
#ifdef WIRE_HAS_END
    OptigaWire->end();
#endif
}


uint16_t OPTIGATrustE::getCertificate(uint8_t pp_cert[], uint32_t& p_length)
{
    uint8_t apdu[] = { HEADER_SPACE, OID_CERTIFICATE };
    CreateHeader(apdu, OPTIGA_CMD_GET_DATA_OBJECT, OPTIGA_PARAM_READ_DATA,
                       sizeof(apdu) - OPTIGA_CMD_HEADER_LEN);

    if (SendApdu(apdu, sizeof(apdu)))
    {
        return IFX_I2C_STACK_ERROR;
    }
    // Determine true length without trailing zero bytes
    if (m_optiga_rx_buffer[OPTIGA_CMD_HEADER_LEN] == 0x30)
    {
        // ASN1 Sequence
        if (m_optiga_rx_buffer[OPTIGA_CMD_HEADER_LEN + 1] == 0x82)
        {
            // ASN1 Extended Length UINT16
            uint8_t* temp = (uint8_t*)(m_optiga_rx_buffer + OPTIGA_CMD_HEADER_LEN);
            p_length = ((m_optiga_rx_buffer[OPTIGA_CMD_HEADER_LEN + 2] << 8)
                        | m_optiga_rx_buffer[OPTIGA_CMD_HEADER_LEN + 3]) + 4;

            //in this case if the p_length gives you the correct length of the certficate, please use (*p_length) instead of LENGTH_CERTIFICATE
            memcpy(pp_cert, temp, p_length);

            return IFX_I2C_STACK_SUCCESS;
        }
    }
    return IFX_I2C_STACK_ERROR;
}

uint16_t OPTIGATrustE::getRandom(uint16_t length, uint8_t p_random[])
{
    uint8_t apdu[] = { HEADER_SPACE, length >> 8, length };
    CreateHeader(apdu, OPTIGA_CMD_GET_RANDOM, 0x00, 2);

    if (p_random == NULL || length < 0x0008 || length > 0x100)
    {
        return IFX_I2C_STACK_ERROR;
    }

    if (SendApdu(apdu, sizeof(apdu)))
    {
        return IFX_I2C_STACK_ERROR;
    }

    memcpy(p_random, (uint8_t*)(m_optiga_rx_buffer + OPTIGA_CMD_HEADER_LEN), length);
    return IFX_I2C_STACK_SUCCESS;
}

uint16_t OPTIGATrustE::setAuthScheme(void)
{
    uint8_t apdu[] = { HEADER_SPACE, OID_PRIVATE_KEY };
    CreateHeader(apdu, OPTIGA_CMD_SET_AUTH_SCHEME,
                       OPTIGA_AUTH_ECDSA_SECP256R1_SHA256, sizeof(apdu) - OPTIGA_CMD_HEADER_LEN);

    return SendApdu(apdu, sizeof(apdu));
}

uint16_t OPTIGATrustE::getSignature(uint8_t p_message[], uint16_t message_length,
        uint8_t pp_signature[], uint32_t& p_signature_len)
{
    uint8_t apdu[OPTIGA_CMD_HEADER_LEN + OPTIGA_AUTH_MSG_LEN];
    CreateHeader(apdu, OPTIGA_CMD_SET_AUTH_MSG, OPTIGA_PARAM_CHALLENGE,
                       OPTIGA_AUTH_MSG_LEN);

    if (message_length != OPTIGA_AUTH_MSG_LEN)
    {
        return IFX_I2C_STACK_ERROR;
    }
    memcpy(apdu + OPTIGA_CMD_HEADER_LEN, p_message, message_length);
    if (SendApdu(apdu, sizeof(apdu)))
    {
        return IFX_I2C_STACK_ERROR;
    }

    CreateHeader(apdu, OPTIGA_CMD_GET_AUTH_MSG, OPTIGA_PARAM_SIGNATURE, 0);
    if (SendApdu(apdu, OPTIGA_CMD_HEADER_LEN))
    {
        return IFX_I2C_STACK_ERROR;
    }

    uint8_t* temp = (uint8_t*)(m_optiga_rx_buffer + OPTIGA_CMD_HEADER_LEN);
    p_signature_len = m_optiga_rx_len - OPTIGA_CMD_HEADER_LEN;

    memcpy(pp_signature, temp, p_signature_len);

    return IFX_I2C_STACK_SUCCESS;

}


uint16_t OPTIGATrustE::generalGetFunction(uint8_t* responseBuffer, uint32_t& responseLength, uint8_t tag, uint8_t OID)
{
    uint8_t apdu[] = { HEADER_SPACE, tag, OID };
    CreateHeader(apdu, OPTIGA_CMD_GET_DATA_OBJECT, OPTIGA_PARAM_READ_DATA, sizeof(apdu) - OPTIGA_CMD_HEADER_LEN);
    if (SendApdu(apdu, sizeof(apdu)))
    {
        return IFX_I2C_STACK_ERROR;
    }
    uint32_t length = *(m_optiga_rx_buffer + 3);
    if (length == 0)
    {
        return IFX_I2C_STACK_ERROR;
    }
    memcpy(responseBuffer, (uint8_t*)(m_optiga_rx_buffer + OPTIGA_CMD_HEADER_LEN), length);
    responseLength = length;

    return IFX_I2C_STACK_SUCCESS;
}


uint16_t OPTIGATrustE::getLcsg(uint8_t responseBuffer[], uint32_t& responseLength)
{
    return generalGetFunction(responseBuffer, responseLength, OPTIGA_OID_TAG, LCS_G);
}

uint16_t OPTIGATrustE::getGlobalSecurityStatus(uint8_t responseBuffer[], uint32_t& responseLength)
{
    return generalGetFunction(responseBuffer, responseLength, OPTIGA_OID_TAG, SECURITY_STATUS_G);
}

uint16_t OPTIGATrustE::getCoprocessorId(uint8_t responseBuffer[], uint32_t& responseLength)
{
    return generalGetFunction(responseBuffer, responseLength, OPTIGA_OID_TAG, COPROCESSOR_UID);
}

uint16_t OPTIGATrustE::getSleepModeActivationDelay(uint8_t responseBuffer[], uint32_t& responseLength)
{
    return generalGetFunction(responseBuffer, responseLength, OPTIGA_OID_TAG, SLEEP_MODE_ACTIVATION_DELAY);
}

uint16_t OPTIGATrustE::getCurrentLimitation(uint8_t responseBuffer[], uint32_t& responseLength)
{
    return generalGetFunction(responseBuffer, responseLength, OPTIGA_OID_TAG, CURRENT_LIMITATION);
}

uint16_t OPTIGATrustE::getSecurityEventCounter(uint8_t responseBuffer[], uint32_t& responseLength)
{
    return generalGetFunction(responseBuffer, responseLength, OPTIGA_OID_TAG, SECURITY_EVENT_COUNTER);
}

uint16_t OPTIGATrustE::getLcsa(uint8_t responseBuffer[], uint32_t& responseLength)
{
    return generalGetFunction(responseBuffer, responseLength, OPTIGA_APP_TAG, LCS_A);
}

uint16_t OPTIGATrustE::getAppSecurityStatus(uint8_t responseBuffer[], uint32_t& responseLength)
{
    return generalGetFunction(responseBuffer, responseLength, OPTIGA_APP_TAG, SECURITY_STATUS_A);
}

uint16_t OPTIGATrustE::getLastErrorCodes(uint8_t responseBuffer[], uint32_t& responseLength)
{
    return generalGetFunction(responseBuffer, responseLength, OPTIGA_APP_TAG, ERROR_CODES);
}

//for the project specific device public key certificate, the OPTIGA return 01234......6401234.....64..... until the recieving buffer of the transport layer is full.

uint16_t OPTIGATrustE::generalSetFunction(uint8_t* dataToSet, uint32_t length, uint8_t tag, uint8_t OID)
{
    //this length is based on the length of the header, the length of the data to set and the length of the write offset, tag and oid (which is 4)
    uint32_t apduLength = (uint32_t)OPTIGA_CMD_HEADER_LEN + length + (uint32_t)4 ;
    uint8_t apdu[apduLength];
    CreateHeader(apdu, OPTIGA_CMD_SET_DATA_OBJECT, OPTIGA_PARAM_WRITE_DATA, sizeof(apdu) - OPTIGA_CMD_HEADER_LEN);

    //initialize the tag,oid and the write offset on the apdu that will be sent
    apdu[4] = tag;
    apdu[5] = OID;
    apdu[6] = 0x00;
    apdu[7] = 0x00;

    //we give apdu + 8 here because the headers initialized above take up the index from 0 to 7. We copy the data provided in dataToSet to the apdu that we send to the Optiga Trust E
    memcpy(apdu + 8, dataToSet, length);

    if (SendApdu(apdu, sizeof(apdu)))
    {
        return IFX_I2C_STACK_ERROR;
    }

    return IFX_I2C_STACK_SUCCESS;
}

uint16_t OPTIGATrustE::setLcsg(uint8_t dataToSet[])
{
    return generalSetFunction(dataToSet, (uint32_t)1, OPTIGA_OID_TAG,  LCS_G);
}

uint16_t OPTIGATrustE::setGlobalSecurityStatus(uint8_t dataToSet[])
{
    return generalSetFunction(dataToSet, (uint32_t)1, OPTIGA_OID_TAG,  SECURITY_STATUS_G);
}

/*
 * Allowed values from 20-255. Default value of 0x14
 */
uint16_t OPTIGATrustE::setSleepModeActivationDelay(uint8_t dataToSet[])
{
    return generalSetFunction(dataToSet, (uint32_t)1, OPTIGA_OID_TAG,  SLEEP_MODE_ACTIVATION_DELAY);
}

/*
 * Allowed values from 9-15. Default value of 0x09
 */
uint16_t OPTIGATrustE::setCurrentLimitation(uint8_t dataToSet[])
{
    return generalSetFunction(dataToSet, (uint32_t)1, OPTIGA_OID_TAG,  CURRENT_LIMITATION);
}

uint16_t OPTIGATrustE::setLcsa(uint8_t dataToSet[])
{
    return generalSetFunction(dataToSet, (uint32_t)1, OPTIGA_APP_TAG,  LCS_A);
}

uint16_t OPTIGATrustE::setAppSecurityStatus(uint8_t dataToSet[])
{
    return generalSetFunction(dataToSet, (uint32_t)1, OPTIGA_APP_TAG,  SECURITY_STATUS_A);
}

uint16_t OPTIGATrustE::setCertificate(uint8_t dataToSet[], uint32_t length)
{
    return generalSetFunction(dataToSet, length, OPTIGA_OID_TAG, OPTIGA_OID_INFINEON_CERT);
}

