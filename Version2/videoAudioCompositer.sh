#!/bin/bash
vFirstTime=$(head -n 1 videostart.txt)
aFirstTime=$(cat alog.txt | grep -o -P '(?<=start: ).*(?=, bitrate:)')
offset=$(echo "$vFirstTime-$aFirstTime" | bc)
ffmpeg -r 20  -i video.avi -ss 00:00:0$offset -i audio.mp3 final.mp4 -y