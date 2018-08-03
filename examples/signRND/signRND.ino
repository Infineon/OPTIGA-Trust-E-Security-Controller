/**
 *  This example gets the signature from the Optiga Trust E according random number generated and the authentication scheme given.
 *  The following protocol must be followed:
 *  1. generate random number of length 16
 *  2. Set the authentication scheme
 *  3. The Optiga Trust E will then generate a signature of length 64 based on the random number and authentication scheme
 */
#include "OPTIGATrustE.h"

// Trust E Opject
OPTIGATrustE TrustE = OPTIGATrustE();

//Global variables that keep track of whether all the actions to be carried out have been carried out successfully
bool Random_Number_Check = false;
bool Auth_Scheme_Check = false;
bool Signature_Check = false;

//length of the random number has to be 16 according to the protocol for getting a signature
uint16_t LEN_RANDOM = 16;

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
    // put your main code here, to run repeatedly:
    while (!(Random_Number_Check && Auth_Scheme_Check && Signature_Check))
    {
        //initialize the global variables
        Random_Number_Check = false;
        Auth_Scheme_Check = false;
        Signature_Check = false;

        // if application opened
        if (actionSuccess == 0)
        {
            // buffer to store the random number
            uint8_t randomBuff[LEN_RANDOM];
            actionSuccess = TrustE.getRandom(LEN_RANDOM, randomBuff);

            // successfully got the random number
            if (actionSuccess == 0)
            {
                Random_Number_Check = true;

                // print the random number
                Serial.println("Random Number:");
                for (uint16_t i = 0; i < LEN_RANDOM; i++)
                {
                    Serial.print(randomBuff[i], HEX);
                    Serial.print(" ");
                }
                Serial.println();
            }
            else
            {
                Serial.println("Error while retrieving random number");
            }

            actionSuccess = TrustE.setAuthScheme();

            //successfully set the authentication scheme
            if (actionSuccess == 0)
            {
                Auth_Scheme_Check = true;
            }
            else
            {
                Serial.println("Error while setting authentication scheme");
            }

            //buffer where the signature will be stored. The length of the signature is 64.
            uint8_t finalSign[64];
            //where the length of the signature will be stored
            uint32_t signLen = 0;

            //gets the signature
            actionSuccess = TrustE.getSignature(randomBuff, 16, finalSign, signLen);
            //successfully got the signature
            if (actionSuccess == 0)
            {
                Signature_Check = true;

                //print the signature
                Serial.println("Signature:");
                for (uint16_t j = 0; j < signLen; j++)
                {
                    Serial.print(finalSign[j], HEX);
                    Serial.print(" ");

                    if ((j + 1) % 20 == 0)
                    {
                        Serial.println();
                    }
                }
            }
            else
            {
                Serial.println("Error in retrieving signature");
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
