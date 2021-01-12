#!/bin/bash

FOLDER_NAME=`date +'%Y-%m-%d_%H-%M-%S'`
ELP_EXPOSURE=30
MAIN_EXPOSURE=350

usage() 
{
	echo "This script runs all the processes to collect the raw data for the video compositor"
	echo "Options:"
	echo "-h	show help"
	echo "-f	folder the raw data will be written to (default: YYYY-MM-DD_HH-mm-SS)"
	echo "-e	the exposure the elp cameras will be set to (default: 30)"
	echo "-m	the exposure the main camera will be set to (default: 350)" 
}

while getopts 'f:e:m:h' flag; do
	case "${flag}" in
		f) FOLDER_NAME="${OPTARG}" ;;
		e) ELP_EXPOSURE="${OPTARG}" ;;		
		m) MAIN_EXPOSURE="${OPTARG}" ;;
		h) usage; exit 1 ;;
	esac
done

cd "$(dirname "$0")"

bash CleanUpAfterCrash.sh #Makes sure the background processes aren't already running

CONFIG=$(cat config.txt | grep -o '^[^#]*' | sed 's/[[:blank:]]*$//')
FPS=$(echo "$CONFIG" | grep FPS | sed 's/^.*=//')
PUBLISH_FRAME_RATE=$(echo "$CONFIG" | grep PUBLISH_FRAME_RATE | sed 's/^.*=//')
PUBLISH_SCALE_FACTOR=$(echo "$CONFIG" | grep PUBLISH_SCALE_FACTOR | sed 's/^.*=//')
MASTER_DIR=$(echo "$CONFIG" | grep MASTER_DIR | sed 's/^.*=//')
VIDEO_DIR=$MASTER_DIR/UncompositedFiles/$FOLDER_NAME
mkdir -m775 $VIDEO_DIR
STARTING_DIR=$(pwd)
AUDIO_CARD_NO=$(arecord -l | grep -o "card.." | tr -d "\n" | tail -c 1)

#Parses the config file below
TR=$(echo "$CONFIG" | grep "TOP_RIGHT=")
TR=${TR: -1}
TL=$(echo "$CONFIG" | grep "TOP_LEFT=")
TL=${TL: -1}
BL=$(echo "$CONFIG" | grep "MAIN_CAMERA=")
BL=${BL: -1}
TR_FILE=$(v4l2-ctl --list-devices | grep -A1 "$TR):" | tail -1)
TL_FILE=$(v4l2-ctl --list-devices | grep -A1 "$TL):" | tail -1)
BL_FILE=$(v4l2-ctl --list-devices | grep -A1 "$BL):" | tail -1)
TR_ALPHA=$(echo "$CONFIG".txt | grep TOP_RIGHT_ALPHA | sed 's/^.*=//')
TR_BETA=$(echo "$CONFIG" | grep TOP_RIGHT_BETA | sed 's/^.*=//')
TL_ALPHA=$(echo "$CONFIG" | grep TOP_LEFT_ALPHA | sed 's/^.*=//')
TL_BETA=$(echo "$CONFIG" | grep TOP_LEFT_BETA | sed 's/^.*=//')
WIDTH=$(echo "$CONFIG" | grep QUAD_WIDTH | sed 's/^.*=//')
HEIGHT=$(echo "$CONFIG" | grep QUAD_HEIGHT | sed 's/^.*=//')

#Changes the camera settings below
#v4l2-ctl -d $TR_FILE --set-ctrl=focus_auto=0
#v4l2-ctl -d $TR_FILE --set-ctrl=focus_absolute=0
v4l2-ctl -d $TR_FILE --set-ctrl=exposure_auto=1
v4l2-ctl -d $TR_FILE --set-ctrl=exposure_absolute=$ELP_EXPOSURE
v4l2-ctl -d $TR_FILE --set-ctrl=gamma=10
#v4l2-ctl -d $TL_FILE --set-ctrl=focus_auto=0
#v4l2-ctl -d $TL_FILE --set-ctrl=focus_absolute=0
v4l2-ctl -d $TL_FILE --set-ctrl=exposure_auto=1
v4l2-ctl -d $TL_FILE --set-ctrl=exposure_absolute=$ELP_EXPOSURE
v4l2-ctl -d $TL_FILE --set-ctrl=gamma=10
#v4l2-ctl -d $BL_FILE --set-ctrl=focus_auto=0
#v4l2-ctl -d $BL_FILE --set-ctrl=focus_absolute=0
v4l2-ctl -d $BL_FILE --set-ctrl=exposure_auto=1
v4l2-ctl -d $BL_FILE --set-ctrl=exposure_absolute=$MAIN_EXPOSURE

cd $VIDEO_DIR
ffmpeg -f alsa -i hw:$AUDIO_CARD_NO audio.mp3 -y &> alog.txt & #for the genius camera audio
sleep .18 && echo "`date +%s%6N` / 1000000" | bc -l > alog.txt & #for other camera's audio
#arecord -f s16_LE --device hw:1 audio.mp3 &> /dev/null & #for other camera's audio
$STARTING_DIR/build/videoCompositer $TL_FILE $TR_FILE $BL_FILE $FPS $TR_ALPHA $TR_BETA $TL_ALPHA $TL_BETA $WIDTH $HEIGHT $PUBLISH_FRAME_RATE $PUBLISH_SCALE_FACTOR &
COMPOSITER_PID=$!

trap "break" SIGINT
while kill -0 $COMPOSITER_PID 2> /dev/null; do
    sleep 1
done

#killall arecord & #for the other camera's audio
pkill -f ffmpeg #for the genius camera

#Kills the cpp program and makes sure its gone before ending
while kill -s SIGINT $COMPOSITER_PID 2> /dev/null; do
    sleep 1
done
