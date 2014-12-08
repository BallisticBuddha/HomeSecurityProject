HomeSecurityProject
===================

This is an Arduino based Home Secirity System that logs events that occur in ones home.

The system contains two pieces, the Arduino controlled device, and a backed server which can run on any hardware of your choosing that can run Linux and the software stack we chose to build on top of the OS. During the development of this project, we used a RasberryPi as our backend server, but any network enabled hardware should work.

System Components
-----------------
**Mounted Unit**
* 1x Arduino Mega 2560 (R3)
    * Unfortinately, we failed to successfully make the device fit on an Uno, given it's limited 2K of memory and only one hardware serial that is taken up by the USB to write messages to the serial monitor that are very useful for debugging.
    * [Buy Link](http://www.amazon.com/Arduino-MEGA-2560-R3/dp/B006H0DWZW/ref=sr_1_1?ie=UTF8&qid=1417997688)
* 1x W5100 Ethernet Shield
    * [Buy Link](http://www.amazon.com/gp/product/B00EU7447Y/ref=oh_aui_search_detailpage?ie=UTF8&psc=1)
* 1x PCF8574P I/O Expander
    * [Buy Link](http://www.amazon.com/gp/product/B00DEEW2E6/ref=oh_aui_search_detailpage?ie=UTF8&psc=1)
* 1x 4x4 Universal Membrane Keypad
    * [Buy Link](http://www.amazon.com/Universial-Switch-Keypad-Keyboard-Arduino/dp/B008A30NW4/ref=sr_1_1?ie=UTF8&qid=1417999010)
* 1-8x PIR Motion sensors
    * [Buy Link](http://www.amazon.com/Pyroelectric-Infrared-Motion-Sensor-Detector/dp/B008AESDSY/ref=sr_1_1?ie=UTF8&qid=1418001312)
* 1x Common Cathode RGB LED
    * [Buy Link](http://www.amazon.com/gp/product/B006S21SAK/ref=oh_aui_search_detailpage?ie=UTF8)
* 3x 410 Ohm Resistors
* 1x Adafruit TTL Serial JPEG camera
    * [Buy Link](http://www.adafruit.com/product/397)
* 2x 10 KOhm Resistors
    * Should come included with the camera, used as a voltage divider for the camera's Rx line
* Any extra misc. electronics equipment (wires, breadboards, perfboards, soldering tools, mounting screws, etc.)

**Backend Server**
* Any network enabled hardware that can run Linux (RaspberryPi B+ is a good choice)

**Connectivity**
* Ethernet cables/switch to connect the two devices together
    * Both devices are configured using static IPv4 addresses by default
* Default Addresses
    * Mounted Unit: 192.168.1.48
    * Backend Server: 192.168.1.49

Basic Info
----------
The purpose of this system is for the user to place the arduino based device at some ingress/egress point in one's home, with a keypad, camera, and indicator light facing the point of interest. The device is armed and disarmed using a passcode that is set up on the web interface for the backend server. Once a user is set up, they may arm or disarm the device using their userID and passocde. The device gives 15 seconds for the user to input a correct userID/passcode after the attempted arm or disarm has started. Up to 8 PIR sensors are used to detect motion and trigger the alarm when in the armed state

**To arm the device**

1. The device is in the disarmed state (green light)
2. Press the 'A' key on the keypad
3. Enter the user's userID
4. Press the '\*' key
5. Enter the user's passcode
6. Press the '\#' key
7. The light should start flashing yellow, in 15 seconds the device will be armed
8. The arm event is logged on the server along with a picture taken at the start of this sequence

**Triggering the alarm**

1. The device is in the armed state (orange light)
2. Motion is detected by any one of the sensors
3. The device will wait 15 seconds for the user to input a correct userID/passcode
4. If a valid userID & passcode is not entered in time, the device will go into the alarmed state (flashing red light)
5. The alarm event is logged on the server along with a picture taken at the start of this sequence

**To disarm the device**

1. The device is in the armed state (orange light)
2. Motion is detected by any one of the sensors (alternativly, press 'D' on the keypad)
3. The device will wait 15 seconds for the user to input a correct userID/passcode
4. If a valid userID & passcode is entered in time, the device will go into the disarmed state (green light)
5. The disarm event is logged on the server along with a picture taken at the start of this sequence

This was made for academic purposes only, and is not intended to be the only point of security in one's home. There are still bugs, and things may go wrong. Additionally, all communications between the device and the server are unencrypted, meaning that userIDs and passcodes can be intercepted by a MITM attack if run on an unsecure network.

We are not liable for any incidents caused by, or not detected by this system, and do not expect users of this system to rely on this as a comprehensive home security system.
