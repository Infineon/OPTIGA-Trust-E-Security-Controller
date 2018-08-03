/**
 *  This example gets the sleep mode activation delay value, changes it and gets the new value stored in the Optiga Trust E.
 */
#include "OPTIGATrustE.h"

// Trust E Opject
OPTIGATrustE TrustE = OPTIGATrustE();

//Global variables that keep track of whether all the actions to be carried out have been carried out successfully
bool First_Get_Check = false;
bool Second_Get_Check = false;
bool Set_Check = false;

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
    while (!(First_Get_Check && Second_Get_Check && Set_Check))
    {
        // initialize the global variables
        First_Get_Check = false;
        Second_Get_Check = false;
        Set_Check = false;

        //variable to keep track of the length of the response recieved
        uint32_t respLen = 0;

        // if application opened
        if (actionSuccess == 0)
        {
            // Array where the value will be stored after the get function
            uint8_t getArray[1] = {0};

            // Array with the value that you wish to set to is stored. For sleep mode activation delay, the valid values range from 20-255 or 0x14-0xFF.
            uint8_t setArray[1] = {0x21};

            actionSuccess = TrustE.getSleepModeActivationDelay(getArray, respLen);

            // Successfully retrieved the current value
            if (actionSuccess == 0)
            {
                Serial.println("initial value:");
                Serial.println(*getArray, HEX);
                First_Get_Check = true;
            }
            else
            {
                Serial.println("Error in getting initial value");
                break;
            }

            //Sets the value
            actionSuccess = TrustE.setSleepModeActivationDelay(setArray);

            //Success in setting value
            if (actionSuccess == 0)
            {
                Serial.println("value set");
                Set_Check = true;
            }
            else
            {
                Serial.println("Error in setting value");
                break;
            }

            // Gets the value after setting the new value
            actionSuccess = TrustE.getSleepModeActivationDelay(getArray, respLen);

            //successfully retrieved the new value
            if (actionSuccess == 0)
            {
                Serial.println("new Value:");
                Serial.println(*getArray, HEX);
                Second_Get_Check = true;
            }
            else
            {
                Serial.println("Error in getting final value");
                break;
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
