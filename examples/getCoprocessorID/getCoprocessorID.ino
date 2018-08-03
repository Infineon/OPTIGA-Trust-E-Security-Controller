/**
 *  This example gets the Coprocessor ID stored in the Optiga Trust E.
 */

#include "OPTIGATrustE.h"

// Trust E Opject
OPTIGATrustE TrustE = OPTIGATrustE();

// global variable to keep track of whether the ID has been successfully acquired
bool acquire = true;
// variable that keeps track of whether an action has been successful.
// if an action is successful, 0 is returned. Otherwise 1 is returned.
uint16_t actionSuccess = 0;

void setup()
{
    delay(1000);
    Serial.begin(9600);

    // starts the connection with the Optiga Trust E and opens the Optiga Trust E application. Needs to be done before carrying out any operation in the Optiga Trust E
    actionSuccess = TrustE.begin();

    // if application opened
    if (actionSuccess == 0)
    {
        Serial.println("Init done!");
    }
    else
    {
        Serial.println("Error while initializing Optiga");
    }
}

// The loop function is called in an endless loop
void loop()
{
    // if the ID has not yet been successfully acquired
    if (acquire)
    {
        // variable that keeps track of the response Length from the action that is carried out
        uint32_t respLen = 0;

        // if application opened
        if (actionSuccess == 0)
        {
            // buffer where the response will be stored.
            uint8_t respBuff[27];

            // this action returns the coprocessor ID
            actionSuccess = TrustE.getCoprocessorId(respBuff, respLen);

            Serial.println("Coprocessor ID:");

            // if the action has been carried out successfully
            if (actionSuccess == 0)
            {
                acquire = false;
                for (uint16_t i = 0; i < respLen; i++)
                {
                    Serial.print(respBuff[i], HEX);
                    Serial.print(" ");
                }
                Serial.println();
            }
        }
        else
        {
            // Restart Optiga
            actionSuccess = TrustE.reset();
        }
    }
    delay(1000);
}
