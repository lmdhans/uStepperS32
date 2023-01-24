# uStepper S32

The library contains support for driving the stepper S32, reading out encoder data. A few examples are included to show the functionality of the library.
The library is supported and tested with in Arduino IDE 1.8.15.

For more information, visit www.ustepper.com

## How to upload software to the uStepper S32

Download the STM32 Cube Programmer from this URL: https://www.st.com/en/development-tools/stm32cubeprog.html.
Once downloaded install the STM32 Cube Programmer.
Download the Arduino IDE (1.8.8+) directly from: https://www.arduino.cc/en/software 
Once downloaded and installed follow the Hardware and library installations as described below.

You are now ready to upload software to the uStepper S32 via the Arduino IDE.
Please do as follows:
- Insert your uStepper S32 board and verify it shows up under "port" in the tools menu.
- Load your programe and compile it.
- Once error free do as follows on the uStepper S32:
  - Press and hold down the "boot" switch.
  - Press the "reset" switch down and release it again.
  - Release the "boot" switch.
- Upload software to the uStepper S32 by selecting "Upload" in the Arduibo IDE.

The "boot" - "reset" switch operation is needed each time a new software is uploaded.

## Installation

Installation is split into two parts - Hardware and Library. Both are required to use the uStepper S boards.

### Hardware Installation 

To add hardware support for uStepper in the Arduino IDE (1.8.8+) do the following:
 - Open Arduino
 - Go to "File->preferences"
 - Almost at the bottom there is a field stating: "Additional Boards Manager URLs" insert this url: https://raw.githubusercontent.com/uStepper/uStepperSTM32Hardware/master/package.json
 - Press OK
 - Go to "Tools->Board->Boards Manager..."
 - Go to the bottom (after it has loaded new files) select "uStepper STM32 boards" and press install

You have now added uStepper hardware support and should be able to select uStepper STM32 boards under tools -> boards.

### Library Installation
For now the library installation is as follows:
- Download the uStepper S32 library as a ZIP file from this URL: https://github.com/uStepper/uStepperS32
- Unzip the library and manually copy it to the Arduino "libraries". The location of the Arduino "libraries" can be found in the Arduino preferences under "Sketchbook location".

In the near future the libraries will be available via the Arduino IDE library manager.
<!---To add the uStepper S32 library do the following:
- Open Arduino IDE (Version 1.8.8 or above)
- Go to "Sketch->Include Library->Manage Libraries..."
- Search for "uStepper S32"
- Select "uStepper S32" and press install
- Close Library Manager --->

## Documentation
The documentation for this library can be found at the following URL:

## Known bugs

## Change Log
0.1.1:	
- Fixed wifiGui example

0.1.0:	
- Initial release

<a rel="license" href="http://creativecommons.org/licenses/by-nc-sa/4.0/"><img alt="Creative Commons License" style="border-width:0" src="https://i.creativecommons.org/l/by-nc-sa/4.0/88x31.png" /></a><br /><span xmlns:dct="http://purl.org/dc/terms/" property="dct:title">uStepper</span> by <a xmlns:cc="http://creativecommons.org/ns#" href="www.ustepper.com" property="cc:attributionName" rel="cc:attributionURL">ON Development</a> is licensed under a <a rel="license" href="http://creativecommons.org/licenses/by-nc-sa/4.0/">Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License</a>.
