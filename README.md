# Power over WIFI

## Introduction
Hey I created this project to power up my PC from remote locations.
Because my PC uses a intel Gen 13 core, I want to shut it completley off.
The much more easy alternative for you would be the Power over LAN function that most
Mainboards support. But I don't want to run my PC even on low power 24/7.

## Structure
**src**: C Application which runs on an ESP32

**schematic**: KiCAD schematic files

## Notes
To run this on your ESP, please read the README in src, it explains how you can
set the SSID and Password to your own.

## Ideas
- add a virtual keyboard usb-driver to the setup to switch to different OSs on startup
    - It should wait on startup until the systemd-boot/grub bootloader lets you decide which os to use then
      input arrow key signals from the ESP to switch to the desired OS
      