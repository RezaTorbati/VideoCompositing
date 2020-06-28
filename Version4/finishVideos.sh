#! /bin/bash

MASTER_DIR=$(cat config.txt | grep MASTER_DIR | sed 's/^.*=//')
FPS=$(cat config.txt | grep FPS | sed 's/^.*=//')

cd $MASTER_DIR/UncompositedFiles

for file in $(ls -d */ | sort)
do
	file=${file: : -1}
	cd $MASTER_DIR/UncompositedFiles/$file
	vFirstTime=$(head -n 1 videostart.txt)
	vFinalTime=$(tail -n 1 videostart.txt)
	aFirstTime=$(cat alog.txt | grep -o -P '(?<=start: ).*(?=, bitrate:)') #for genius
	#aFirstTime=$(cat alog.txt) #for the elp camera audio
	offset=$(echo "$vFirstTime-$aFirstTime" | bc)
	duration=$(echo "$vFinalTime-$vFirstTime" | bc)
	ffmpeg -r $FPS -i video.avi -ss 00:00:0$offset -i audio.mp3 -t $duration ../../FinishedVideos/finishedVideo$file.mp4 -y
	cd ..
	mv $file ../OldFiles/
	#rm videostart.txt alog.txt audio.mp3 video.avi
	#cd ..
	#rmdir $file	
done

echo "all videos composited"
