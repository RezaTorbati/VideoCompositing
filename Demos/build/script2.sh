#!/bin/bash
vFirstTime=$(head -n 1 timestamps.txt)
aFirstTime=$(cat testatime.txt | grep -o -P '(?<=start: ).*(?=, bitrate:)')
offset=$(echo "$vFirstTime-$aFirstTime" | bc)
ffmpeg -r 20  -i cppDemo.avi -ss 00:00:0$offset -i atest.mp3 output.mp4 -y