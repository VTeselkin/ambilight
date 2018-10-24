# Ambilight on Hyperion with Arduino

[![N|Solid](https://bitbucket.org/Vteselkin/ambilight/raw/8fef3fe736072ae82a785db1c3afc21764fc6980/git/logo.jpg)](https://nodesource.com/products/nsolid)

[![Build Status](https://travis-ci.org/VTeselkin/ambilight.svg?branch=master)](https://travis-ci.org/VTeselkin/ambilight/) [![Only 32 Kb](https://badge-size.herokuapp.com/Naereen/StrapDown.js/master/strapdown.min.js)](https://github.com/Vteselkin/ambilight/blob/master/ambilight.ino) [![Maintenance](https://img.shields.io/badge/Maintained%3F-no-red.svg)](https://bitbucket.org/lbesson/ansi-colors) [![HitCount](http://hits.dwyl.io/Vteselkin/ambilight.svg)](http://hits.dwyl.io/Vteselkin/ambilight/) [![GitHub issues](https://img.shields.io/github/issues/Naereen/StrapDown.js.svg)](https://GitHub.com/Naereen/StrapDown.js/issues/) [![GitHub release](https://img.shields.io/github/release/Naereen/StrapDown.js.svg)](https://github.com/VTeselkin/ambilight/blob/master/ambilight.ino) [![GitHub version](https://badge.fury.io/gh/Naereen%2FStrapDown.js.svg)](https://github.com/VTeselkin/ambilight/blob/master/ambilight.ino)
[![GitHub license](https://img.shields.io/github/license/Naereen/StrapDown.js.svg)](https://github.com/VTeselkin/ambilight/blob/master/LICENSE.txt)

# Thinking: How to make Ambilight clone for Raspberry ?
Very long title… but worth reading if you plan to make ambilight clone for your LED TV. After a few days of "hard work" I will try to make complete guide what RGB diodes and software to choose and how to solve all traps while trying to make Ambilight for you LED TV!

# Idea
Idea is to add LED RGB strip behind your LED TV that will change colours according to video that your player (Raspberry Pi) is playing. Why? Because you can :) and it is easier for your eyes at night.

# Software for Ambilight Arduino Raspberry clone
First there is some "fresh" information. XBMC is now called Kodi. Basically it is part of the software responsible for multimedia playing. If you have stand alone PC hardware you can install it additionally to you operating system. But if we are talking about Raspberry Pi, most reasonable is to choose distribution (SD card image) that comes with preinstalled XBMC. Now there are several options:
- Raspbmc available here: http://www.raspbmc.com/index.html In last month it was announced that project is closing down, but it is stable, all plugins work so still it is valid option
- OSMC (Open Source Media Center) is new OS that inherits Raspbmc. It is available here: https://osmc.tv It boots very fast and has fresh look. Alpha version that we tested are not yet stable (that's why they call them alpha) but it looks very promising. Bad news that there is no support for Boblight, and good news that they will support Hyperion. So you can check their progress.
- OpenElec – RECOMMENDED – you can get image for your SD card here: http://openelec.tv/get-openelec It is very stable, supports Boblight and it is possible to add Hyperion too. Only drawback – you can not change root password easily (?!)

# Software for LED backlight

Basically we have two major options: Boblight or Hyperion They have slightly different approaches, while Boblight consisting of daemon – server (boblightd) and XBMC boblight plugin as client. Plugin is responsible for all calculations (grabs edge pixels of screen) and serialises all data to boblight daemon that then sends all data to your Arduino "driver". This has big drawback – impact on your CPU and mayor issue – lag, and we want RGB lights running lightning fast!
# Why choose Hyperion?
For start it is newer and more promising. But here are some real advantages:

- It is very fast (almost no delay compared to almost 1sec lag with boblight)
- Priority channels – while you are watching video you can use mobile app to override colors or play effect!
- Did I mention mobile app for iOS ?
# How to install Hyperion?
```sh
curl -L --output install_hyperion.sh --get https://raw.githubusercontent.com/tvdzwan/hyperion/master/bin/install_hyperion.sh

sh ./install_hyperion.sh
```
Detailed installation guide available here: https://github.com/hyperion-project/hyperion/wiki

### Hyperion JSON configuration
You can be guru and write your own config file, or just get and use latest Hyperion JSON configurator:
https://sourceforge.net/projects/hyperion-project/files/latest/download?source=files
# How to change  Hyperion configuration or troubleshoot?
If setup is not working, first check if the hyperion is running by typing:
```sh
ps -a | grep hyper
```
If you want to see and debug hyperion messages run it instead with:
```sh
/storage/hyperion/bin/hyperiond.sh /storage/.config/hyperion.config.json
```
There is an easy way to test it using one of shell commands below (for solid colour or Rainbow effect):
```sh
/storage/hyperion/bin/hyperion-remote.sh --priority 50 --color red --duration 5000 

/storage/hyperion/bin/hyperion-remote.sh --effect "Rainbow swirl fast" --duration 3000
```
If it still does not work you can always edit configuration, adjust it

```sh
nano /storage/.config/hyperion.config.json
```
# Hardware for "RGB delight"
#### What RGB diodes to choose?
There are many options for RGB LED strip. Idea is to have controlable one – to be more specific RGB diodes that you can individually control. So be careful while you are ordering your 5m pack. What model should you choose? I was thinking about WS2801, WS 2811, WS2812, WS2812b. `Keeping it short, you should go for WS2812b IP65.`

## Why choose WS2812b with IP65 and 3M stick tape
#### Advantages:

- They use just 3 wires! +5v, GND and DATA line.
- WS2812 have chip integrated in 5050 RGB diode!
- WS2812b have reverse voltage protection (it is easy to mix two wires and kill LED strip if you do not have b version)
- IP65 gives you silicone over diodes, I like that more than IP67 (plastic tube). It is good that your LED strip is protected from dust etc.
- 3M stick tape helps you to stick the diodes direct to TV (if you are less brave and would like to make frame, check some websites)
they come in 3 variants 30LED, 60LED or 144LED / m. I think that 60LED/m provide optimal brightness.
#### WS2812 main drawbacks:
 - Hard to controll It can not (or can but it is not easy to do) be controlled directly from Raspberry Pi2! You must use Arduino. Why? Because it needs 800kHz signal and your Raspberry Pi is not running realtime OS, so any interrupt will brake the timing and cause problems and unreliable behaviour.
 - They need lots of power at 5V DC supply on packaging bag they are stated at 90W. But for 5 meters 10A 5V will be enough. I have measured 3A to 5A while all 5m were on. If you have shorter length maybe 5A supply will be enough. Max power calculation is 5m x 60 LED = 300 LED. each LED is 3 separate colors (Red Green Blue = RGB) so 3x 20mA. Total 300x60mA=18A current. Voltage 5V (beware not to use 12V!!) Giving total power 18A * 5V = 90W ! Have in mind that equivalent of regular bulb wuould be multiplied by e.g. 7, so max brightness would give you 90W * 7 = 630W (advice: do not stare directly at your LEDs!)
 
 ## What Arduino to choose for Ambilight

Beware! There are some project that suggest direct connection of Raspberry Pi to Arduino via serial pin (RX TX) or SPI. Some arduinos are working with 5V logic level and Raspberry is using 3.3V and you will probably require TTL logic level shifters.The safe way is to connect them with USB cable (micro for Arduino Leonardo, mini for Arduino Nano or stanrad USB for Arduino Mega)
When you connect Arduino Nano to Raspberry it will be detected as /dev/ttyUSB0. If you use Arduino Mega it will be /dev/ttyACM0. How to check that? Just login via SSH to your Raspberry and type: 
```sh
ls -al /dev/tty*
```
Which will give you something like:
 ```sh
 crw--w----    1 root     tty         4,   8 Jan  1  1970 /dev/tty8 
 crw--w----    1 root     tty         4,   9 Jan  1  1970 /dev/tty9 
 crw-rw----    1 root     dialout   204,  64 Jan  1  1970 /dev/ttyAMA0 
 crw-rw----    1 root     dialout   188,   0 Mar 28 18:52 /dev/ttyUSB0 
 ```
 You will see ttyUSB0 or ttyACM0 and then you will know what to enter in config files. If your USB serial is missing you can re-plug Arduino. You can always check with command: 
 ```sh 
 dmesg
 ```
 Which will give you result like:
  ```sh 
 [   70.912700] usb 1-1.3: new full-speed USB device number 9 using dwc_otg 
 [   71.032687] usb 1-1.3: New USB device found, idVendor=0403, idProduct=6001 
 [   71.032724] usb 1-1.3: New USB device strings: Mfr=1, Product=2, SerialNumber=3 
 [   71.032741] usb 1-1.3: Product: FT232R USB UART 
 [   71.032756] usb 1-1.3: Manufacturer: FTDI 
 [   71.032771] usb 1-1.3: SerialNumber: AH01KPB9 
 [   71.072351] usbcore: registered new interface driver usbserial 
 [   71.072562] usbcore: registered new interface driver usbserial_generic 
 [   71.072696] usbserial: USB Serial support registered for generic 
 [   71.092861] usbcore: registered new interface driver ftdi_sio 
 [   71.092992] usbserial: USB Serial support registered for FTDI USB Serial Device 
 [   71.093405] ftdi_sio 1-1.3:1.0: FTDI USB Serial Device converter detected 
 [   71.093584] usb 1-1.3: Detected FT232RL 
 [   71.094575] usb 1-1.3: FTDI USB Serial Device converter now attached to ttyUSB0
 ```
 # What about Raspberry Pi Arduino USB serial port speed? 
 ## `Use 500.000 !`
 # What about Arduino sketch?
 To work correctly in the sketch you need to make several important changes.
 **Firstly**, override several parameters:
```sh
#define STARTCOLOR 0xff8000  // LED color at startup 0xff8000 is orange in hex code. 
#define BLACK      0x000000  // LED color BLACK
#define DATAPIN    6         // Datapin
#define LEDCOUNT   108       //  Number of LEDs used in your system
#define SHOWDELAY  20        // Delay in micro seconds before showing
#define BAUDRATE   500000    // Serial port speed
#define BRIGHTNESS 100        // Max. brightness in %
```
**Must change DATAPIN and LEDCOUNT if needed!**
DATAPIN - Port number to which the data channel is connected to the LED strip
LEDCOUNT - The number of used LEDs in the tape
STARTCOLOR - LED color at startup 0xff8000 is orange in hex code. Pick your own here:
http://www.w3schools.com/colors/colors_picker.asp
BAUDRATE - **`Put same value in Hyperion`**

**Secondly**, change prefix at the start of the transmission:
```sh
const char prefix[] = { 0x41, 0x64, 0x61, 0x00, 0x6B, 0x3E }; 
```
##### `The first 4 bytes will never change: 0x41, 0x64, 0x61, 0x00`
**The next byte is equal to the number of LED - 1 --> (108-1)=107. 107 transformed in HEX. 107 in hex is 6B (use google).
The last byte is equal to the XOR value of the calculated value just above and 0x55 (byte just calculated (6B) XORED with 0x55) = 3E     
Use this link http://xor.pw/ and in input 1 put 55 and in input 2 put your HEX value.**






License
----

MIT


**Free Software, Hell Yeah!**


