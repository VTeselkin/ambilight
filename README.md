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
