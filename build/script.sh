sleep 3s && ffmpeg -i cppDemo.avi -f alsa -i hw:1 -c:v copy -map 0:v:0 -map 1:a:0 -c:a aac -b:a 192k test.mp4 -y &
./cppDemo