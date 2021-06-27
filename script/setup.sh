#!/bin/sh

sudo apt update
sudo apt install apache2-dev 
sudo apt install libopencv-dev
sudo apt install cmake
sudo apt install libgtest-dev
cd /usr/src/gtest/
sudo cmake CMakeLists.txt
sudo make
#sudo cp *.a /usr/lib
