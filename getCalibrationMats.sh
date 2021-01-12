#! /bin/bash

cd "$(dirname "$0")"
CONFIG=$(cat config.txt | grep -o '^[^#]*' | sed 's/[[:blank:]]*$//')
CALIBRATION_DIR=$(echo "$CONFIG" | grep CALIBRATION_DIR | sed 's/^.*=//')
WIDTH=$(echo "$CONFIG" | grep QUAD_WIDTH | sed 's/^.*=//')
HEIGHT=$(echo "$CONFIG" | grep QUAD_HEIGHT | sed 's/^.*=//')
CORRECTION_FACTOR=$(echo "$CONFIG" | grep CORRECTION_FACTOR | sed 's/^.*=//')
CALIBRATION_COEFFICIENT=$(echo "$CONFIG" | grep CALIBRATION_COEF | sed 's/^.*=//')

currentDir=$(pwd)

cd $CALIBRATION_DIR
rm calibrationMats.bin
cd $currentDir/build
./calibrate $CALIBRATION_DIR $WIDTH $HEIGHT $CORRECTION_FACTOR $CALIBRATION_COEFFICIENT
