# Pill Reminder Project
Objective The objective of this project was to create a pill reminder and scheduler. Using OLED and UART functions, the user can upload a pill reminder schedule and once reminder mode is selected, the device displays the time, which runs at 1000 times normal and count down to the next time the patient must take a pill. If a pill was missed, all the LEDs flash 10 times and the OLED will display which pill was missed. 

## Materials
* MCU:  Chipkit PIC32 microcontroller and shield
* IDE: MPLAB X

## Instructions
* Connect microcontroller to computer, build and upload project onto MCU
* Will need to enter schedule from a terminal. I used TeraTerm

## Abstract
This project uses a combination of the chipkit’s OLED and UART functionalities to program a pill reminder and scheduler system. The program allows users to upload a pill schedule and then acts as a reminder, lighting up LEDs if a pill was not taken. This project involved the integration of input, display and communication components. This project implemented predefined functions, plus general C programing to create a working application. The PC via UART functions was mainly used to collect input from user, while the device itself, through LED and OLED was the source of displaying information to the user. The project involved controlling the display on the OLED and implementing communications between the PIC32 and CPU, using UART. The codes were built and run, and the application was run successfully.  
 
### Background
This project incorporates many of the aspects learnt from previous labs, including the LAT, PORT, and TRIS registers, as well as the implementation of switches, timers, and oscillators. The universal asynchronous receiver-transmitter is a device used for asynchronous serial communications. This allows data to be easily transferred between transmitter and receiver at any time since the two do not need to be synchronized to a common clock. Data is sent between devices as soon as the user types it. The OLED stands for Organic light emitting diodes and it is the main display used for this project. 
 
### Future work
Something to consider in improving this code would be enabling the user to upload more complex schedules, and use buffers to accept a string of integer input without the user having to press enter after each. As for the application itself, it would be very interesting to explore more functionalities, for example, adding sound to make an alarm when they must take each pill. 
