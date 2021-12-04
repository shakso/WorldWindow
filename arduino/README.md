There are two components to the Arduino build - they can be run independently of each other since they both talk to a common message bus.

The WorldMap sketch provides the physical interface to change locations - all this is detailed at https://alexshakespeare.com

The ThermalControl sketch provides the perspective shifting component to move the image as a person moves in front of the thermal camera.