# Picker upper bot
Picker upper bot is a semi-autonomous robot that can be commanded to move and pick up objects as well as autonomously navigate an environment using line detectors and sonar. It is a distributed system composed of the VEX ARM Cortex Microcontroller, a Beaglebone Black and a PC workstation. The cortex software is written in a variant of C called Robot C, and it handles the robot's behaviours such as collision avoidance, line following, driving, and picking up/dropping objects. The Beaglebone client runs on Debian 11 and captures driving and arm commands from the terminal and sends it to the cortex via UART. A PC workstation is used to access the Beaglebone Debian terminal using SSH in order to send commands to the Beaglebone client. The three-tier architecture was driven by hardware constraints. The VEX Cortex has no WiFi capability, so the BeagleBone acts as a wireless bridge, receiving commands over SSH from a PC and forwarding them to the cortex over UART. The Cortex runs its own behaviour layer concurrently with manual commands, using a range finder to prevent collision.

## Getting started

### Cortex
To build and test the cortex code, use RobotC.

### Beaglebone
To build and test the Beaglebone client, use Visual Studio. Ensure that the Linux development for C++ workload is installed. A guide can be found [here](https://devblogs.microsoft.com/cppblog/linux-development-with-c-in-visual-studio/).  
Please note: Ensure that the following command has been run in the SSH window that will be running the code on the BeagleBone:

```
config-pin P9.13 uart
config-pin P9.11 uart
```
This will configure the pins to UART.
