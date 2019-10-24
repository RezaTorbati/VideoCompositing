#ffmpeg -y -i cppDemo.avi -i atest.mp3 \
#  -filter_complex "[0]atrim=0:2[Apre];[0]atrim=5,asetpts=PTS-STARTPTS[Apost];\
#                   [Apre][1][Apost]concat=n=3:v=0:a=1" out.mp4

vFirstTime=$(head -n 1 timestamps.txt)
aFirstTime=$(cat testatime.txt | grep -o -P '(?<=start: ).*(?=, bitrate:)')
offset=$(echo "$vFirstTime-$aFirstTime" | bc)
ffmpeg -r 20  -i cppDemo.avi -ss 00:00:0$offset -i atest.mp3 output.mp4 -y
#ffmpeg -r 20 -itsoffset $offset -i cppDemo.avi -i atest.mp3 output.mp4 -y
