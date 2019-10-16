ffmpeg -r 20 -i cppDemo.avi -i atest.mp3 output.mp4 -y
#sleep 10s && ffmpeg -i cppDemo.avi -i atest.wav -c:v copy -map 0:v:0 -map 1:a:0 -c:a aac -b:a 192k -r 20 output.mp4 -y &
#sleep 5s && ffmpeg -i cppDemo.avi -i atest.mp3 -c copy -map 0:v:0 -map 1:a:0 output.mp4
#sleep 5s && ffmpeg -i cppDemo.avi -f alsa -i hw:1 -c:v copy -map 0:v:0 -map 1:a:0 -c:a aac -b:a 192k test.mp4 -y) &
#sleep 5s && ffmpeg -i cppDemo.avi -i atest.wav -c:v copy -map 0:v:0 -map 1:a:0 -c:a aac -b:a 192k test.mp4 -y &
#sleep 5s && ffmpeg -i cppDemo.avi -i atest.mp4 -c:v copy -map 0:v:0 -map 1:a:0 test.mp4 -y &