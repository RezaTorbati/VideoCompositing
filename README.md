# VideoCompositing
The code that I am writing as a research assistant for Dr. Fagg at the University of Oklahoma. <br>
The idea of the program is to record video from three different cameras, composit them into one frame with a resolution of 640x480 and at a minimum of 20FPS on a raspberry pi4. <br>
The program is set up to record individual frames as fast as possible and write them to disk and then a seperate program that is run after the initial data collecting program will automatically take the frames and correct their distortion (the 170degree FOV camera frames are extremely distorted), combine the images into a video and add audio. <br>
The hardware used for this project is two 170degree fov ELP Cameras, a 120FOV genuis camera, a raspberry pi4, and an optional external USB hard drive. <br>
<br>
Note: the development of this code is currently being done using SVN. I am now only periodically uploading large changes to Github so that I have an easy way of accessing them without the SVN and so others can easily view it.
