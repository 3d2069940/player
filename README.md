# Gstreamer Media Player

## Features

* Equalizer
* Delay
* Low Pass/High Pass Filter
* Pitch
* Compressor/Expander
* Audio panorama
* Audio visualization (still in development)
* Saving presets for any of the effects

## Installation

### Linux

```
sudo apt-get update && sudo apt-get upgrade -y 
```
```
sudo apt-get install pkg-config qt5-default libgstreamer1.0-dev yaml-cpp gstreamer1.0-plugins-bad gstreamer1.0-plugins-good gstreamer1.0-plugins-base
```
```
git clone https://github.com/3d2069940/gstreamer-media-player.git
```
```
mkdir build && cd build && qmake -o Makefile ../gstreamer_player.pro && make
```
