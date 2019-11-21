#!/bin/bash
cd src
gcc -c -fPIC -L/opt/ffmpeg/lib -I/opt/ffmpeg/include/ ffmpeg.c -lavcodec -lavformat -lavfilter -lavdevice -lswresample -lswscale -lavutil -o ffmpeg.o
g++ -O3 -Wall -shared -std=c++11 syszuxav.cpp -fPIC `python3 -m pybind11 --includes` ffmpeg.o -L/opt/ffmpeg/lib -lavcodec -lavformat -lavfilter -lavdevice -lswresample -lswscale -lavutil -o syszuxav`python3-config --extension-suffix`
#python2 issue
g++ -O3 -Wall -shared -std=c++11 syszuxav.cpp -fPIC `python -m pybind11 --includes` -I/usr/include/python2.7 ffmpeg.o -L/opt/ffmpeg/lib -lavcodec -lavformat -lavfilter -lavdevice -lswresample -lswscale -lavutil -o syszuxav`python-config --extension-suffix`

cd ..
cp src/*.so syszuxav/

python setup.py sdist upload

