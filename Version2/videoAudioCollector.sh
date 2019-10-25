#!/bin/bash
ffmpeg -f alsa -i hw:1 audio.mp3 -y &> alog.txt &
./videoCompositer