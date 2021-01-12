#! /bin/bash
pkill -f ffmpeg #Assumes all ffmpeg programs should be killed
pkill -f build/videoCompositer #will not write end time or empty the queue so the final video will likely not composit correctly so do this as a last resort
