#! /bin/bash

cd "$(dirname "$0")"
STARTING_DIR=$(pwd)
CONFIG=$(cat config.txt | grep -o '^[^#]*' | sed 's/[[:blank:]]*$//')
MASTER_DIR=$(echo "$CONFIG" | grep MASTER_DIR | sed 's/^.*=//')
CALIBRATION_DIR=$(echo "$CONFIG" | grep CALIBRATION_DIR | sed 's/^.*=//')
FPS=$(echo "$CONFIG" | grep FPS | sed 's/^.*=//')
CALIBRATION_COEF=$(echo "$CONFIG" | grep CALIBRATION_COEF | sed 's/^.*=//')
CORRECTION_FACTOR=$(echo "$CONFIG" | grep CORRECTION_FACTOR | sed 's/^.*=//')
WIDTH=$(echo "$CONFIG" | grep WIDTH | sed 's/^.*=//')
HEIGHT=$(echo "$CONFIG" | grep HEIGHT | sed 's/^.*=//')

cd $MASTER_DIR/UncompositedFiles

for file in $(ls -d */ | sort)
do
	echo ""
	echo "Starting video"
	file=${file: : -1}
	cd $MASTER_DIR/UncompositedFiles/$file
	vFirstTime=$(head -n 1 videostart.txt)
	vFinalTime=$(tail -n 1 videostart.txt)
	aFirstTime=$(cat alog.txt | grep -o -P '(?<=start: ).*(?=, bitrate:)') #for genius
	#aFirstTime=$(cat alog.txt) #for the other cameras audio
	offset=$(echo "$vFirstTime-$aFirstTime" | bc)
	duration=$(echo "$vFinalTime-$vFirstTime" | bc)
	$STARTING_DIR/build/dewarper $CALIBRATION_DIR $CALIBRATION_COEF $CORRECTION_FACTOR $WIDTH $HEIGHT $FPS
	#gdb $STARTING_DIR/build/dewarper --args $STARTING_DIR/build/dewarper $CALIBRATION_DIR $CALIBRATION_COEF $CORRECTION_FACTOR $WIDTH $HEIGHT $FPS
	
	ffmpeg -r $FPS -i correctedVideo.avi -ss 00:00:0$offset -i audio.mp3 -t $duration ../../FinishedVideos/$file-video.mp4 -y
	cd ../
	mv $file ../OldFiles/
done

echo "all videos composited"
