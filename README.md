# Gstreamer Media Player

## Features

* Equalizer
* Delay
* Low Pass/High Pass Filter
* Pitch
* Compressor/Expander
* Audio panorama
* Audio visualization
* Saving presets for any of the effects

## Installation

### Linux

```
sudo apt-get update && sudo apt-get upgrade -y 
```
```
sudo apt-get install cmake pkg-config qt5-default libgstreamer1.0-dev yaml-cpp gstreamer1.0-plugins-{base,good,bad} libtag1
```
```
git clone https://github.com/3d2069940/gstreamer-media-player.git
```
```
mkdir build && cd build && cmake .. && build
```
