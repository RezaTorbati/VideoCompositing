#!/bin/bash
killall "build/videoCompositer /dev/video2 /dev/video0 /dev/video4"
killall ffmpeg
rm videostart.txt video.avi alog.txt audio.mp3