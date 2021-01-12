#! /bin/bash
cd "$(dirname "$0")"
cd build
make
cd ..
bash compositVideo.sh
