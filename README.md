# Bird Feeder Project
Bird feeder project including source code and system specifications. This project utilized two Moteino microcontrollers, a Raspberry Pi Zero, a NEMA-17 stepper motor, a micro servo, motor controller, 3-d printed remote, custom PCBs, camera, and acrylic casing. An RF communication protocol was developed using the RFM69 tranceivers on the Moteinos to establish communication between the remote and the main device. The main device consists of custom acrylic casing and a circulating tray that is actuated using the motor controller and the NEMA-17 stepper motor. 

The remote control has several functions including moving to a specific position and mode toggling (automatic vs manual). It also has a dispense option which sends a command to the device prompting a micro servo-controlled flap to move to the open position. 

In automatic mode, the Raspberry Pi parses video frames from the camera to determine whether a specific bird is present. The image recognition system was implemented using the OpenCV computer vision platform and Haar cascades. We used serial communication via the UART to allow the Raspberry Pi Zero to update the Moteino. If a valid bird is detected by the Raspberry Pi Zero in automatic mode, it updates the Moteino and the Moteino actuates the NEMA-17 stepper motor to the unique dispensing compartment specific to that bird.

Custom PCBs were designed for this project using DipTrace and a PCB etching machine. Video documenting this project can be found here:
https://youtu.be/XW5JDQq0iwM

Due to time and accessibility constraints (the birds were from the Point Defiance Zoo & Aquarium and full access to the birds was limited to one session) we were only able to create one Haar cascade.
