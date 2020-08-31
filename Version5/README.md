# Dependencies
Requires OpenCV, FFmpeg, three cameras (tested on two of https://www.amazon.com/ELP-degree-fisheye-autofocus-Industrial/dp/B01GHNSH6W/ref=sr_1_fkmr0_2?keywords=ELP%2B170%2Bdegree%2BFisheye%2BLens%2BWide%2BAngle%2BWebcam%2BMini%2B5mp%2BAutofocus%2BCmos%2BOV5640%2BColour%2BUSB%2BCamera&qid=1579131016&sr=8-2-fkmr0&th=1 and one https://www.amazon.com/Genius-120-degree-Conference-Webcam-WideCam/dp/B0080CE5M4/ref=sr_1_2?crid=11ABUVYRH9YYE&keywords=genius+widecam+f100&qid=1579126348&s=electronics&sprefix=genuis+wid%2Celectronics%2C176&sr=1-2) 

# Set up
Ensure that everything in config.txt is accurate for your environment <br>
Note: HD_DIR and NFS_DIR are stand in lines that may be safely deleted. MASTER_DIR is the only important directory in config <br>
Ensure that the MASTER_DIR given in config contains three folders called FinishedVideos, OldFiles and UncompositedFiles

# Running
Ensure that config.txt is properly filled out, the cameras are plugged in. <br>
Create a build file in this directory and run "cmake .." in it. <br>
Navigate back to this directory and run "bash run.sh" to compile the code and run it <br>
To stop recording type 'q' in the terminal and then press Enter <br>
To run the post processing program run "bash finishVideos.sh" in this directory <br>
Please note that the post processing may take awhile <br>
The final video will be in the FinishedVideos folder in the MASTER_DIR specified in config.txt
