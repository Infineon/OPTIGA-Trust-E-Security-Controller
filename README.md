# OPTIGA&trade; Trust E
<img src="https://github.com/Infineon/Assets/blob/master/Pictures/OPTIGA-Trust-E.png">

Library of Infineon's security solution [OPTIGA&trade; Trust E](https://www.infineon.com/cms/en/product/security-smart-card-solutions/optiga-embedded-security-solutions/optiga-trust/optiga-trust-e-sls-32aia/) for Arduino.

## Summary
[OPTIGA&trade; Trust E](https://www.infineon.com/dgdl/Infineon-OPTIGA%20TRUST%20E%20SLS%2032AIA-DS-v01_00-EN.pdf?fileId=5546d4625e37f35a015e56cfb42f5b81) is a security solution based on a secure micrcontroller.
Each device is shipped with a unique elliptic-curve keypair which is used for One-way authentication. OPTIGA&trade; Trust E allows for easy integration into existing PKI infrastructure.


## Key Features and Benefits
* High-end security controller
* Turnkey security solution
* Cryptographic support: ECC256 (NIST curve), SHA256
* Memory: 3kB user memory
* Temperature range (up to): -40 – 85 °C.
* Current consumption: configurable
* Package: PG USON 10 2 (3 x 3mm)
* Interface: I2C 

## Hardware
The wiring to your arduino board depends on the [evaluation board](https://www.infineon.com/cms/en/product/evaluation-boards/optiga-trust-e-eval-kit/) or the 
[Shield2GO](https://www.infineon.com/cms/en/product/evaluation-boards/s2go-security-optiga-e/) you are using. At the moment there is no arduino pin compatible board available from Infineon.

## Installation

### Integration of Library
Please download this repository from GitHub by clicking on the following field in the latest [release](https://github.com/Infineon/OPTIGA-Trust-E-Security-Controller/releases) of this repository or directly [here](https://github.com/Infineon/OPTIGA-Trust-E-Security-Controller/archive/V1.0.0.zip):


To install the OPTIGA&trade; Trust E library in the Arduino IDE, please go now to **Sketch** > **Include Library** > **Add .ZIP Library...** in the Arduino IDE and navigate to the downloaded .ZIP file of this repository. The library will be installed in your Arduino sketch folder in libraries and you can select as well as include this one to your project under **Sketch** > **Include Library** > **OPTIGATrustE**.

![Install Library](https://raw.githubusercontent.com/infineon/assets/master/Pictures/Library_Install_ZIP.png)

## Usage
Please follow the example sketches in the /examples directory in this library to learn more about the usage of OPTIGA&trade; Trust E.