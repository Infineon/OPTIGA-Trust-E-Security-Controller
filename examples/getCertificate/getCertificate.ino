/**
 *  This example gets the Public Device Certificate stored in the Optiga Trust E.
 */
#include "OPTIGATrustE.h"

// Trust E Opject
OPTIGATrustE TrustE = OPTIGATrustE();

// global variable to keep track of whether the certificate has been successfully acquired
bool acquire = true;
// buffer where the response will be stored.
uint8_t respBuff[1024];
// variable that keeps track of whether an action has been successful.
// if an action is successful, 0 is returned. Otherwise 1 is returned.
uint16_t actionSuccess = 0;

void setup()
{
    // put your setup code here, to run once:
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

void loop()
{
    // if the certificate has not successfully been acquired
    if (acquire)
    {
        //variable where the response length is stored
        uint32_t respLen = 0;

        // if application opened
        if (actionSuccess == 0)
        {

            // gets the certificate
            actionSuccess = TrustE.getCertificate(respBuff, respLen);

            // if the certificate has been successfully gotten.
            if (actionSuccess == 0)
            {
                acquire = false;
				
                // Prints out the entire certificate
                Serial.println("Certificate:");
                for (uint16_t i = 0; i < respLen; i++)
                {
                    if (respBuff[i] < 16)
                    {
                        Serial.print("0");
                    }
                    Serial.print(respBuff[i], HEX);
                    Serial.print(" ");

                    if ((i + 1) % 29 == 0)
                    {
                        Serial.println();
                    }
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
