#!/bin/bash

#Parses the config file below
TR=$(cat config.txt | grep TOP_RIGHT)
TR=${TR: -1}
TL=$(cat config.txt | grep TOP_LEFT)
TL=${TL: -1}
BL=$(cat config.txt | grep BOTTOM_LEFT)
BL=${BL: -1}
TR_FILE=$(v4l2-ctl --list-devices | grep -A1 "$TR):" | tail -1)
TL_FILE=$(v4l2-ctl --list-devices | grep -A1 "$TL):" | tail -1)
BL_FILE=$(v4l2-ctl --list-devices | grep -A1 "$BL):" | tail -1)

echo "Press 'q' to stop"
ffmpeg -f alsa -i hw:1 audio.mp3 -y &> alog.txt &
build/videoCompositer $TR_FILE $TL_FILE $BL_FILE &
while : ; do #waits for user to press q before continueing
	read -n 1 k <&1
	if [[ $k = q ]] ; then
		break
	fi
done
killall build/videoCompositer
killall ffmpeg

vFirstTime=$(head -n 1 videostart.txt)
aFirstTime=$(cat alog.txt | grep -o -P '(?<=start: ).*(?=, bitrate:)')
offset=$(echo "$vFirstTime-$aFirstTime" | bc)
ffmpeg -r 20  -i video.avi -ss 00:00:0$offset -i audio.mp3 finishedVideo.mp4 -y

rm videostart.txt video.avi alog.txt audio.mp3