#!/bin/bash
ffmpeg -f alsa -i hw:1 atest.mp3 -y &> testatime.txt &
./cppDemo
