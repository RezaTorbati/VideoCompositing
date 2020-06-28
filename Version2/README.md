# Version2 Video Compsiter
## To Compile:
First make sure OpenCV is installed and built <br>
Navigate to VideoCompositing/Version2 <br>
Now run the following commands to compile: <br>
mkdir build && cd build <br>
cmake .. <br>
make <br>

## To run:
Ensure that 3 USB cameras are plugged into the ports specified in config.txt <br>
NOTE: to find where the ports the cameras are in you can use "v4l2-ctl --list-devices" <br>
Run the compositVideo.sh file. <br>
Please make sure to end the program with 'q' and not ctrl C or bad things may happen
