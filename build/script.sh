#!/bin/bash
sleep 5s && ffmpeg -f alsa -i hw:1 atest.mp3 -y &



./cppDemo
