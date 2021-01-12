#! /bin/bash

cd "$(dirname "$0")"
STARTING_DIR=$(pwd)
CONFIG=$(cat config.txt | grep -o '^[^#]*' | sed 's/[[:blank:]]*$//')
MASTER_DIR=$(echo "$CONFIG" | grep MASTER_DIR | sed 's/^.*=//')
FPS=$(echo "$CONFIG" | grep FPS | sed 's/^.*=//')
WIDTH=$(echo "$CONFIG" | grep WIDTH | sed 's/^.*=//')
HEIGHT=$(echo "$CONFIG" | grep HEIGHT | sed 's/^.*=//')

cd $MASTER_DIR/UncompositedFiles

for file in $(ls -d */ | sort)
do
	echo ""
	echo "Starting video"
	file=${file: : -1}
	cd $MASTER_DIR/UncompositedFiles/$file
	$STARTING_DIR/build/compress $WIDTH $HEIGHT $FPS && rm images.bin
	cd ../
done

echo "all videos compressed"
