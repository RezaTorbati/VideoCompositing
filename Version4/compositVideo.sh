#!/bin/bash

FPS=$(cat config.txt | grep FPS | sed 's/^.*=//')
MASTER_DIR=$(cat config.txt | grep MASTER_DIR | sed 's/^.*=//')
VIDEO_DIR=$MASTER_DIR/UncompositedFiles/$(date +%s)
mkdir $VIDEO_DIR
STARTING_DIR=$(pwd)

#Parses the config file below
TR=$(cat config.txt | grep TOP_RIGHT)
TR=${TR: -1}
TL=$(cat config.txt | grep TOP_LEFT)
TL=${TL: -1}
BL=$(cat config.txt | grep GENIUS)
BL=${BL: -1}
TR_FILE=$(v4l2-ctl --list-devices | grep -A1 "$TR):" | tail -1)
TL_FILE=$(v4l2-ctl --list-devices | grep -A1 "$TL):" | tail -1)
BL_FILE=$(v4l2-ctl --list-devices | grep -A1 "$BL):" | tail -1)

#Changes the camera settings below
#v4l2-ctl -d $TR_FILE --set-ctrl=focus_auto=0
#v4l2-ctl -d $TR_FILE --set-ctrl=focus_absolute=0
v4l2-ctl -d $TR_FILE --set-ctrl=exposure_auto=1
v4l2-ctl -d $TR_FILE --set-ctrl=exposure_absolute=18
v4l2-ctl -d $TR_FILE --set-ctrl=gamma=10
#v4l2-ctl -d $TL_FILE --set-ctrl=focus_auto=0
#v4l2-ctl -d $TL_FILE --set-ctrl=focus_absolute=0
v4l2-ctl -d $TL_FILE --set-ctrl=exposure_auto=1
v4l2-ctl -d $TL_FILE --set-ctrl=exposure_absolute=18
v4l2-ctl -d $TL_FILE --set-ctrl=gamma=10
#v4l2-ctl -d $BL_FILE --set-ctrl=focus_auto=0
#v4l2-ctl -d $BL_FILE --set-ctrl=focus_absolute=0
v4l2-ctl -d $BL_FILE --set-ctrl=exposure_auto=1
v4l2-ctl -d $BL_FILE --set-ctrl=exposure_absolute=500 #genius camera doesn't need the same level of tuning

cd $VIDEO_DIR

echo "Press 'q' to stop"
ffmpeg -f alsa -i hw:1 audio.mp3 -y &> alog.txt & #for the genius camera audio
#sleep .18 && echo "`date +%s%6N` / 1000000" | bc -l > alog.txt & #for elp audio
#arecord -f s16_LE --device hw:1 audio.mp3 &> /dev/null & #for elp audio
$STARTING_DIR/build/videoCompositer $TL_FILE $TR_FILE $BL_FILE $FPS
#killall arecord & #for the elp camera
pkill -f ffmpeg #for the genius camera
